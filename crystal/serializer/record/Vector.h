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

#include <cstdlib>
#include <stdexcept>

#include "crystal/serializer/record/OffsetPtr.h"

namespace crystal {

template <class T>
class Vector {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  Vector() noexcept = default;

  ~Vector() {
    if (offset_) {
      std::free(offset_.get());
      offset_ = nullptr;
    }
  }

  reference at(size_t pos) {
    if (pos >= size()) {
      throw std::out_of_range("string::at");
    }
    return operator[](pos);
  }
  const_reference at(size_t pos) const {
    if (pos >= size()) {
      throw std::out_of_range("string::at");
    }
    return operator[](pos);
  }

  reference operator[](size_t pos) {
    return reinterpret_cast<T*>(offset_ + 1)[pos];
  }
  const_reference operator[](size_t pos) const {
    return reinterpret_cast<T*>(offset_ + 1)[pos];
  }

  char& front() {
    return operator[](0);
  }
  const char& front() const {
    return operator[](0);
  }

  char& back() {
    return operator[](size() - 1);
  }
  const char& back() const {
    return operator[](size() - 1);
  }

  T* data() noexcept = delete;
  const T* data() const noexcept {
    return offset_ ? reinterpret_cast<const T*>(offset_ + 1) : nullptr;
  }

  iterator begin() noexcept = delete;
  const_iterator begin() const noexcept {
    return data();
  }

  iterator end() noexcept = delete;
  const_iterator end() const noexcept {
    return data() + size();
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  size_t size() const noexcept {
    return offset_ ? *offset_ : 0;
  }
  size_t length() const noexcept {
    return size();
  }

  T* reserve(size_t n = 0) {
    uint32_t* p = reinterpret_cast<uint32_t*>(
        std::malloc(n * sizeof(T) + sizeof(uint32_t)));
    *p = n;
    return reinterpret_cast<T*>(p + 1);
  }
  void reset(T* p) {
    this->~String();
    offset_ = reinterpret_cast<uint32_t*>(p) - 1;
  }

  static const size_t npos = -1;

 private:
  OffsetPtr<uint32_t> offset_;
};

} // namespace crystal
