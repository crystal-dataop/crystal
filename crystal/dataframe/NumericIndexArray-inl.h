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

#include "crystal/dataframe/detail/iota.h"
#include "crystal/foundation/Bits.h"

namespace crystal {

template <class T>
NumericIndexArray<T>::NumericIndexArray(size_t capacity) {
  reserve(capacity);
}

template <class T>
NumericIndexArray<T>::~NumericIndexArray() {
  if (ptr_) {
    delete [] ptr_;
  }
}

template <class T>
NumericIndexArray<T>::NumericIndexArray(NumericIndexArray&& other) {
  *this = std::move(other);
}

template <class T>
NumericIndexArray<T>&
NumericIndexArray<T>::operator=(NumericIndexArray&& other) {
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(ptr_, other.ptr_);
  return *this;
}

template <class T>
inline void NumericIndexArray<T>::clear() {
  size_ = 0;
}

template <class T>
inline T* NumericIndexArray<T>::data() const {
  return ptr_;
}

template <class T>
inline size_t NumericIndexArray<T>::size() const {
  return size_;
}

template <class T>
inline size_t NumericIndexArray<T>::capacity() const {
  return capacity_;
}

template <class T>
inline bool NumericIndexArray<T>::empty() const {
  return size_ == 0;
}

template <class T>
inline T* NumericIndexArray<T>::begin() {
  return ptr_;
}

template <class T>
inline const T* NumericIndexArray<T>::begin() const {
  return ptr_;
}

template <class T>
inline T* NumericIndexArray<T>::end() {
  return ptr_ + size_;
}

template <class T>
inline const T* NumericIndexArray<T>::end() const {
  return ptr_ + size_;
}

template <class T>
inline T NumericIndexArray<T>::front() const {
  assert(size_ != 0);
  return ptr_[0];
}

template <class T>
inline T NumericIndexArray<T>::back() const {
  assert(size_ != 0);
  return ptr_[size_ - 1];
}

template <class T>
inline T NumericIndexArray<T>::operator[](int i) const {
  return ptr_[i];
}

template <class T>
inline void NumericIndexArray<T>::push_back(T value) {
  if (size_ == capacity_) {
    reserve(size_ + 1);
  }
  ptr_[size_++] = value;
}

template <class T>
inline void NumericIndexArray<T>::pop_back() {
  if (size_ != 0) {
    --size_;
  }
}

template <class T>
inline void NumericIndexArray<T>::swap(NumericIndexArray& other) {
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(ptr_, other.ptr_);
}

template <class T>
void NumericIndexArray<T>::reserve(size_t size) {
  if (size <= capacity_) {
    return;
  }
  while (capacity_ < size) {
    capacity_ = capacity_ < 16 ? 16 : nextPowTwo(capacity_ + 1);
  }
  T* newData = new T[capacity_];
  if (ptr_) {
    memcpy(newData, ptr_, size_ * sizeof(T));
    delete [] ptr_;
  }
  ptr_ = newData;
}

template <class T>
void NumericIndexArray<T>::resize(size_t size, bool fillZero) {
  if (size > size_) {
    reserve(size);
    T* p = end();
    if (fillZero) {
      memset(p, 0, (size - size_) * sizeof(T));
    } else {
      detail::iota(p, p + size - size_, size_);
    }
  }
  size_ = size;
}

template <class T>
bool NumericIndexArray<T>::merge(NumericIndexArray& other, T offset) {
  reserve(size_ + other.size_);
  if (offset != 0) {
    for (auto i : other) {
      ptr_[size_++] = i + offset;
    }
  } else {
    memcpy(end(), other.ptr_, other.size_ * sizeof(T));
    size_ += other.size_;
  }
  return true;
}

}  // namespace crystal
