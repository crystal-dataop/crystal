/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Modified from folly.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <type_traits>

#include "crystal/foundation/Bits.h"
#include "crystal/foundation/Likely.h"
#include "crystal/foundation/Random.h"

namespace crystal {

template <
    typename Key,
    typename Value,
    typename Allocator,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename IndexType = uint32_t>

struct AtomicUnorderedInsertMap {

  typedef Key key_type;
  typedef Value mapped_type;
  typedef std::pair<Key,Value> value_type;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef Hash hasher;
  typedef KeyEqual key_equal;
  typedef const value_type& const_reference;

  typedef struct ConstIterator {
    ConstIterator(const AtomicUnorderedInsertMap& owner, IndexType slot)
      : owner_(owner)
      , slot_(slot)
    {}

    ConstIterator(const ConstIterator&) = default;
    ConstIterator& operator= (const ConstIterator&) = default;

    const value_type& operator* () const {
      return owner_.slots_[slot_].keyValue();
    }

    const value_type* operator-> () const {
      return &owner_.slots_[slot_].keyValue();
    }

    // pre-increment
    const ConstIterator& operator++ () {
      while (slot_ > 0) {
        --slot_;
        if (owner_.slots_[slot_].state() == LINKED) {
          break;
        }
      }
      return *this;
    }

    // post-increment
    ConstIterator operator++(int /* dummy */) {
      auto prev = *this;
      ++*this;
      return prev;
    }

    bool operator== (const ConstIterator& rhs) const {
      return slot_ == rhs.slot_;
    }
    bool operator!= (const ConstIterator& rhs) const {
      return !(*this == rhs);
    }

   private:
    const AtomicUnorderedInsertMap& owner_;
    IndexType slot_;
  } const_iterator;

  friend ConstIterator;

  /// Constructs a map that will support the insertion of maxSize key-value
  /// pairs without exceeding the max load factor.  Load factors of greater
  /// than 1 are not supported, and once the actual load factor of the
  /// map approaches 1 the insert performance will suffer.  The capacity
  /// is limited to 2^30 (about a billion) for the default IndexType,
  /// beyond which we will throw invalid_argument.
  explicit AtomicUnorderedInsertMap(
      size_t maxSize,
      float maxLoadFactor = 0.8f,
      const Allocator& alloc = Allocator())
    : allocator_(alloc)
  {
    size_t capacity = size_t(maxSize / std::min(1.0f, maxLoadFactor) + 128);
    size_t avail = size_t{1} << (8 * sizeof(IndexType) - 2);
    if (capacity > avail && maxSize < avail) {
      // we'll do our best
      capacity = avail;
    }
    if (capacity < maxSize || capacity > avail) {
      throw std::invalid_argument(
          "AtomicUnorderedInsertMap capacity must fit in IndexType with 2 bits "
          "left over");
    }

    numSlots_ = capacity;
    slotMask_ = nextPowTwo(capacity * 4) - 1;
    mmapRequested_ = sizeof(Slot) * capacity;
    slots_ = reinterpret_cast<Slot*>(allocator_.allocate(mmapRequested_));
  }

  ~AtomicUnorderedInsertMap() {
    allocator_.deallocate(reinterpret_cast<char*>(slots_), mmapRequested_);
  }

  /// Searches for the key, returning (iter,false) if it is found.
  /// If it is not found calls the functor Func with a void* argument
  /// that is raw storage suitable for placement construction of a Value
  /// (see raw_value_type), then returns (iter,true).  May call Func and
  /// then return (iter,false) if there are other concurrent writes, in
  /// which case the newly constructed value will be immediately destroyed.
  ///
  /// This function does not block other readers or writers.  If there
  /// are other concurrent writes, many parallel calls to func may happen
  /// and only the first one to complete will win.  The values constructed
  /// by the other calls to func will be destroyed.
  ///
  /// Usage:
  ///
  ///  AtomicUnorderedInsertMap<std::string,std::string> memo;
  ///
  ///  auto value = memo.findOrConstruct(key, [=](void* raw) {
  ///    new (raw) std::string(computation(key));
  ///  })->first;
  template <typename Func>
  std::pair<const_iterator,bool> findOrConstruct(const Key& key, Func&& func) {
    auto const slot = keyToSlotIdx(key);
    auto prev = slots_[slot].headAndState_.load(std::memory_order_acquire);

    auto existing = find(key, slot);
    if (existing != 0) {
      return std::make_pair(ConstIterator(*this, existing), false);
    }

    auto idx = allocateNear(slot);
    new (&slots_[idx].keyValue().first) Key(key);
    func(static_cast<void*>(&slots_[idx].keyValue().second));

    while (true) {
      slots_[idx].next_ = prev >> 2;

      // we can merge the head update and the CONSTRUCTING -> LINKED update
      // into a single CAS if slot == idx (which should happen often)
      auto after = idx << 2;
      if (slot == idx) {
        after += LINKED;
      } else {
        after += (prev & 3);
      }

      if (slots_[slot].headAndState_.compare_exchange_strong(prev, after)) {
        // success
        if (idx != slot) {
          slots_[idx].stateUpdate(CONSTRUCTING, LINKED);
        }
        return std::make_pair(ConstIterator(*this, idx), true);
      }
      // compare_exchange_strong updates its first arg on failure, so
      // there is no need to reread prev

      existing = find(key, slot);
      if (existing != 0) {
        // our allocated key and value are no longer needed
        slots_[idx].keyValue().first.~Key();
        slots_[idx].keyValue().second.~Value();
        slots_[idx].stateUpdate(CONSTRUCTING, EMPTY);

        return std::make_pair(ConstIterator(*this, existing), false);
      }
    }
  }

  /// This isn't really emplace, but it is what we need to test.
  /// Eventually we can duplicate all of the std::pair constructor
  /// forms, including a recursive tuple forwarding template
  /// http://functionalcpp.wordpress.com/2013/08/28/tuple-forwarding/).
  template <class K, class V>
  std::pair<const_iterator,bool> emplace(const K& key, V&& value) {
    return findOrConstruct(key, [&](void* raw) {
      new (raw) Value(std::forward<V>(value));
    });
  }

  const_iterator find(const Key& key) const {
    return ConstIterator(*this, find(key, keyToSlotIdx(key)));
  }

  const_iterator cbegin() const {
    IndexType slot = numSlots_ - 1;
    while (slot > 0 && slots_[slot].state() != LINKED) {
      --slot;
    }
    return ConstIterator(*this, slot);
  }

  const_iterator cend() const {
    return ConstIterator(*this, 0);
  }

 private:
  enum : IndexType {
    kMaxAllocationTries = 1000, // after this we throw
  };

  enum BucketState : IndexType {
    EMPTY = 0,
    CONSTRUCTING = 1,
    LINKED = 2,
  };

  /// Lock-free insertion is easiest by prepending to collision chains.
  /// A large chaining hash table takes two cache misses instead of
  /// one, however.  Our solution is to colocate the bucket storage and
  /// the head storage, so that even though we are traversing chains we
  /// are likely to stay within the same cache line.  Just make sure to
  /// traverse head before looking at any keys.  This strategy gives us
  /// 32 bit pointers and fast iteration.
  struct Slot {
    /// The bottom two bits are the BucketState, the rest is the index
    /// of the first bucket for the chain whose keys map to this slot.
    /// When things are going well the head usually links to this slot,
    /// but that doesn't always have to happen.
    std::atomic<IndexType> headAndState_;

    /// The next bucket in the chain
    IndexType next_;

    /// Key and Value
    typename std::aligned_storage<sizeof(value_type),
                                  alignof(value_type)>::type raw_;


    ~Slot() {
      auto s = state();
      assert(s == EMPTY || s == LINKED);
      if (s == LINKED) {
        keyValue().first.~Key();
        keyValue().second.~Value();
      }
    }

    BucketState state() const {
      return BucketState(headAndState_.load(std::memory_order_acquire) & 3);
    }

    void stateUpdate(BucketState before, BucketState after) {
      assert(state() == before);
      headAndState_ += (after - before);
    }

    value_type& keyValue() {
      assert(state() != EMPTY);
      return *static_cast<value_type*>(static_cast<void*>(&raw_));
    }

    const value_type& keyValue() const {
      assert(state() != EMPTY);
      return *static_cast<const value_type*>(static_cast<const void*>(&raw_));
    }

  };

  // We manually manage the slot memory so we can bypass initialization
  // (by getting a zero-filled mmap chunk) and optionally destruction of
  // the slots

  size_t mmapRequested_;
  size_t numSlots_;

  /// tricky, see keyToSlodIdx
  size_t slotMask_;

  Allocator allocator_;
  Slot* slots_;

  IndexType keyToSlotIdx(const Key& key) const {
    size_t h = hasher()(key);
    h &= slotMask_;
    while (h >= numSlots_) {
      h -= numSlots_;
    }
    return h;
  }

  IndexType find(const Key& key, IndexType slot) const {
    KeyEqual ke = {};
    auto hs = slots_[slot].headAndState_.load(std::memory_order_acquire);
    for (slot = hs >> 2; slot != 0; slot = slots_[slot].next_) {
      if (ke(key, slots_[slot].keyValue().first)) {
        return slot;
      }
    }
    return 0;
  }

  /// Allocates a slot and returns its index.  Tries to put it near
  /// slots_[start].
  IndexType allocateNear(IndexType start) {
    for (IndexType tries = 0; tries < kMaxAllocationTries; ++tries) {
      auto slot = allocationAttempt(start, tries);
      auto prev = slots_[slot].headAndState_.load(std::memory_order_acquire);
      if ((prev & 3) == EMPTY &&
          slots_[slot].headAndState_.compare_exchange_strong(
              prev, prev + CONSTRUCTING - EMPTY)) {
        return slot;
      }
    }
    throw std::bad_alloc();
  }

  /// Returns the slot we should attempt to allocate after tries failed
  /// tries, starting from the specified slot.  This is pulled out so we
  /// can specialize it differently during deterministic testing
  IndexType allocationAttempt(IndexType start, IndexType tries) const {
    if (CRYSTAL_LIKELY(tries < 8 && start + tries < numSlots_)) {
      return IndexType(start + tries);
    } else {
      IndexType rv;
      if (sizeof(IndexType) <= 4) {
        rv = IndexType(Random::rand32(numSlots_));
      } else {
        rv = IndexType(Random::rand64(numSlots_));
      }
      assert(rv < numSlots_);
      return rv;
    }
  }
};

template <typename T>
struct MutableData {
  mutable T data;
  explicit MutableData(const T& init) : data(init) {}
};

} // namespace crystal
