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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "crystal/foundation/String.h"

namespace crystal {

template <class T>
class Span {
 public:
  typedef T element_type;
  typedef std::remove_cv_t<T> value_type;
  typedef size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* iterator;
  typedef const T* const_iterator;

  Span(const_pointer data, size_type size)
      : data_(data), size_(size) {}
  Span(const_pointer begin, const_pointer end)
      : data_(begin), size_(end - begin) {}

  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_ + size_; }

  const_reference front() const { return data_[0]; }
  const_reference back() const { return data_[size_ - 1]; }
  const_reference operator[](size_type i) const { return data_[i]; }

  const_pointer data() const { return data_; }

  size_type size() const { return size_; }
  size_type size_bytes() const { return size_ * sizeof(element_type); }

  bool empty() const { return size_ == 0; }

  std::string toString() const;

 private:
  const_pointer data_;
  size_type size_;
};

template <class T>
void copy(const T* a, T* b, size_t n) {
  memcpy(b, a, n * sizeof(T));
}

//////////////////////////////////////////////////////////////////////

template <class T>
std::string Span<T>::toString() const {
  return crystal::toString("[", size(), "]", join(',', *this));
}

}  // namespace crystal
