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

#include <iterator>
#include <vector>

#include "crystal/foundation/detail/Iterators.h"

namespace crystal {

template <class T, size_t Size = sizeof(T)>
class DoubleLayerArray {
 public:
  template <class V>
  class Iterator;

  typedef T value_type;
  typedef Iterator<T> iterator;
  typedef Iterator<T const> const_iterator;

  explicit DoubleLayerArray(size_t blockSize = kBlockSize)
      : blockSize_(blockSize),
        blockCapacity_(blockSize / Size) {}

  virtual ~DoubleLayerArray() {}

  DoubleLayerArray(const DoubleLayerArray&) = delete;
  DoubleLayerArray& operator=(const DoubleLayerArray&) = delete;
  DoubleLayerArray(DoubleLayerArray&&) = default;
  DoubleLayerArray& operator=(DoubleLayerArray&&) = default;

  size_t size() const;
  size_t capacity() const;

  bool empty() const;

  T& operator[](size_t i);
  const T& operator[](size_t i) const;

  const_iterator cbegin() const;
  const_iterator cend() const;
  iterator begin();
  iterator end();

  template <class... Args>
  T& emplaceTemp(Args&&... args);
  template <class... Args>
  T& emplace(Args&&... args);

  void reserve(size_t n);
  void resize(size_t n);
  void increment();

  void swap(DoubleLayerArray<T, Size>& other);
  void merge(DoubleLayerArray<T, Size>& other);

 private:
  static constexpr size_t kBlockSize = 1048576ul * 2;

  class Block;

  void appendBlock();

  std::vector<Block> blocks_;
  size_t size_{0};
  size_t capacity_{0};
  size_t blockSize_{kBlockSize};
  size_t blockCapacity_{kBlockSize / Size};
  size_t curBlock_{0};
};

//////////////////////////////////////////////////////////////////////

template <class T, size_t Size>
template <class V>
class DoubleLayerArray<T, Size>::Iterator
    : public detail::IteratorFacade<
      typename DoubleLayerArray<T, Size>::template Iterator<V>, V> {
 public:
  Iterator() {}
  Iterator(DoubleLayerArray<T, Size>* array, size_t pos)
      : array_(array), pos_(pos) {}

  Iterator(Iterator<V> const& other)
      : Iterator(other.array_, other.pos_) {}

  Iterator& operator=(Iterator<V> const& other) {
    array_ = other.array_;
    pos_ = other.pos_;
    return *this;
  }

  V& dereference() const {
    return (*array_)[pos_];
  }

  bool equal(const Iterator& other) const {
    return array_ == other.array_ && pos_ == other.pos_;
  }

  void increment() {
    ++pos_;
  }

 private:
  DoubleLayerArray<T, Size>* array_{nullptr};
  size_t pos_{0};
};

template <class T, size_t Size>
class DoubleLayerArray<T, Size>::Block {
 public:
  Block() {}
  Block(size_t blockSize) {
    data_ = reinterpret_cast<T*>(malloc(blockSize));
  }

  ~Block() {
    if (data_) {
      free(data_);
    }
  }

  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;

  Block(Block&& other) {
    *this = std::move(other);
  }
  Block& operator=(Block&& other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    return *this;
  }

  T& operator[](size_t i) {
    return data_[i];
  }
  const T& operator[](size_t i) const {
    return data_[i];
  }

  T& end() {
    return data_[size_];
  }

  void resize(size_t n) {
    size_ = n;
  }
  void increment() {
    ++size_;
  }

 private:
  T* data_{nullptr};
  size_t size_{0};
};

template <class T, size_t Size>
inline size_t DoubleLayerArray<T, Size>::size() const {
  return size_;
}

template <class T, size_t Size>
inline size_t DoubleLayerArray<T, Size>::capacity() const {
  return capacity_;
}

template <class T, size_t Size>
inline bool DoubleLayerArray<T, Size>::empty() const {
  return size_ == 0;
}

template <class T, size_t Size>
inline T& DoubleLayerArray<T, Size>::operator[](size_t i) {
  return blocks_[i / blockCapacity_][i % blockCapacity_];
}

template <class T, size_t Size>
inline const T& DoubleLayerArray<T, Size>::operator[](size_t i) const {
  return blocks_[i / blockCapacity_][i % blockCapacity_];
}

template <class T, size_t Size>
inline typename DoubleLayerArray<T, Size>::const_iterator
DoubleLayerArray<T, Size>::cbegin() const {
  return const_iterator(this, 0);
}

template <class T, size_t Size>
inline typename DoubleLayerArray<T, Size>::const_iterator
DoubleLayerArray<T, Size>::cend() const {
  return const_iterator(this, size_);
}

template <class T, size_t Size>
inline typename DoubleLayerArray<T, Size>::iterator
DoubleLayerArray<T, Size>::begin() {
  return iterator(this, 0);
}

template <class T, size_t Size>
inline typename DoubleLayerArray<T, Size>::iterator
DoubleLayerArray<T, Size>::end() {
  return iterator(this, size_);
}

template <class T, size_t Size>
template <class... Args>
inline T& DoubleLayerArray<T, Size>::emplaceTemp(Args&&... args) {
  if (size_ == capacity_) {
    appendBlock();
  }
  T& obj = blocks_[curBlock_].end();
  new (&obj) T(std::forward<Args>(args)...);
  return obj;
}

template <class T, size_t Size>
template <class... Args>
inline T& DoubleLayerArray<T, Size>::emplace(Args&&... args) {
  T& obj = emplaceTemp(std::forward<Args>(args)...);
  increment();
  return obj;
}

template <class T, size_t Size>
void DoubleLayerArray<T, Size>::reserve(size_t n) {
  while (n > capacity_) {
    appendBlock();
  }
}

template <class T, size_t Size>
void DoubleLayerArray<T, Size>::resize(size_t n) {
  if (n < size_) {
    curBlock_ = n / blockCapacity_;
    blocks_[curBlock_].resize(n % blockCapacity_);
    size_t i = curBlock_;
    while (++i < blocks_.size()) {
      blocks_[i].resize(0);
    }
    size_ = n;
  } else {
    while (n > size_) {
      emplace();
    }
  }
}

template <class T, size_t Size>
inline void DoubleLayerArray<T, Size>::increment() {
  blocks_[curBlock_].increment();
  ++size_;
  if (size_ % blockCapacity_ == 0) {
    ++curBlock_;
  }
}

template <class T, size_t Size>
inline void DoubleLayerArray<T, Size>::swap(DoubleLayerArray<T, Size>& other) {
  std::swap(blocks_, other.blocks_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

template <class T, size_t Size>
void DoubleLayerArray<T, Size>::merge(DoubleLayerArray<T, Size>& other) {
  for (size_t i = 0; i < other.size(); ++i) {
    emplace(std::move(other[i]));
  }
}

template <class T, size_t Size>
inline void DoubleLayerArray<T, Size>::appendBlock() {
  blocks_.emplace_back(blockSize_);
  capacity_ += blockCapacity_;
}

} // namespace crystal
