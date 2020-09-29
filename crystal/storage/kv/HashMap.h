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

#pragma once

#include <memory>

#include "crystal/memory/SimpleAllocator.h"
#include "crystal/storage/kv/detail/AtomicUnorderedMap.h"

namespace crystal {

template <class K, class V>
class HashMap {
 public:
  struct Alloc {
    void* allocate(size_t size) {
      if (alloc.getMemory()->readOnly()) {
        return alloc.address(kMemStart);
      }
      return alloc.address(alloc.allocate(size));
    }
    void deallocate(void*, size_t) {}

    SimpleAllocator& alloc;
  };

  typedef AtomicUnorderedInsertMap<K, MutableData<V>, Alloc> Map;
  typedef typename Map::const_iterator const_iterator;

  HashMap(size_t maxSize)
      : maxSize_(maxSize) {}

  virtual ~HashMap() {}

  bool init(Memory* memory);

  std::pair<const_iterator,bool> emplace(const K& key, V&& value);

  const_iterator find(const K& key) const;
  const_iterator cbegin() const;
  const_iterator cend() const;

 private:
  size_t maxSize_;
  std::unique_ptr<Map> map_;
  SimpleAllocator alloc_{true};
};

//////////////////////////////////////////////////////////////////////

template <class K, class V>
bool HashMap<K, V>::init(Memory* memory) {
  alloc_.init(memory);
  if (memory->getAllocatedSize() == 0) {
    if (alloc_.allocate(0) == 0) {
      return false;
    }
  }
  map_.reset(new Map(maxSize_, 0.8f, Alloc{alloc_}));
  return true;
}

template <class K, class V>
inline std::pair<typename HashMap<K, V>::const_iterator,bool>
HashMap<K, V>::emplace(const K& key, V&& value) {
  return map_->emplace(key, std::forward<V>(value));
}

template <class K, class V>
inline typename HashMap<K, V>::const_iterator
HashMap<K, V>::find(const K& key) const {
  return map_->find(key);
}

template <class K, class V>
inline typename HashMap<K, V>::const_iterator
HashMap<K, V>::cbegin() const {
  return map_->cbegin();
}

template <class K, class V>
inline typename HashMap<K, V>::const_iterator
HashMap<K, V>::cend() const {
  return map_->cend();
}

} // namespace crystal
