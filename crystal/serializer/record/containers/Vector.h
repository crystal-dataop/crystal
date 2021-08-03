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
#include <initializer_list>
#include <memory>
#include <stdexcept>

#include "crystal/serializer/record/AllocMask.h"
#include "crystal/serializer/record/OffsetPtr.h"

namespace crystal {

template <class T>
class vector;
template <class T>
void serialize(const vector<T>& from, vector<T>& to, void* buffer);

template <class T>
class vector {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  vector() noexcept = default;

  ~vector() {
    if (offset_) {
      uint8_t* old = offset_.get();
      offset_ = nullptr;
      if (!getMask(old)) {
        std::free(old);
      }
    }
  }

  vector(size_t n, const T& value) {
    assign(n, value);
  }
  explicit vector(size_t n) {
    assign(n, T());
  }
  template <class InputIt>
  vector(InputIt first, InputIt last) {
    assign(first, last);
  }
  vector(const vector& other) {
    *this = other;
  }
  vector(vector&& other) noexcept {
    std::swap(offset_, other.offset_);
  }
  vector(std::initializer_list<T> list) {
    assign(list);
  }

  vector& operator=(const vector& other) {
    size_t n = other.size();
    write(n, [&](T* p, size_t) {
      T* it = p;
      for (auto& value : other) {
        new (it++) T(value);
      }
    });
    return *this;
  }
  vector& operator=(vector&& other) noexcept {
    std::swap(offset_, other.offset_);
    return *this;
  }
  vector& operator=(std::initializer_list<T> list) {
    assign(list);
    return *this;
  }

  void assign(size_t n, const T& value) {
    write(n, [&](T* p, size_t n) {
      T* it = p;
      for (size_t i = 0; i < n; ++i) {
        new (it++) T(value);
      }
    });
  }
  template <class It, class Category = typename
            std::iterator_traits<It>::iterator_category>
  void assign(It first, It last) {
    size_t n = last - first;
    write(n, [&](T* p, size_t) {
      for (T* it = p; first != last; ) {
        new (it++) T(*first++);
      }
    });
  }
  void assign(std::initializer_list<T> list) {
    size_t n = list.size();
    write(n, [&](T* p, size_t) {
      T* it = p;
      for (const T& value : list) {
        new (it++) T(value);
      }
    });
  }

  reference at(size_t pos) {
    if (pos >= size()) {
      throw std::out_of_range("vector::at");
    }
    return operator[](pos);
  }
  const_reference at(size_t pos) const {
    if (pos >= size()) {
      throw std::out_of_range("vector::at");
    }
    return operator[](pos);
  }

  reference operator[](size_t pos) {
    return reinterpret_cast<T*>(offset_ + getBytes(offset_))[pos];
  }
  const_reference operator[](size_t pos) const {
    return reinterpret_cast<const T*>(offset_ + getBytes(offset_))[pos];
  }

  T& front() {
    return operator[](0);
  }
  const T& front() const {
    return operator[](0);
  }

  T& back() {
    return operator[](size() - 1);
  }
  const T& back() const {
    return operator[](size() - 1);
  }

  T* data() noexcept {
    return offset_ ? reinterpret_cast<T*>(offset_ + getBytes(offset_))
                   : nullptr;
  }
  const T* data() const noexcept {
    return offset_ ? reinterpret_cast<const T*>(offset_ + getBytes(offset_))
                   : nullptr;
  }

  iterator begin() noexcept {
    return data();
  }
  const_iterator begin() const noexcept {
    return data();
  }

  iterator end() noexcept {
    return data() + size();
  }
  const_iterator end() const noexcept {
    return data() + size();
  }

  bool empty() const noexcept {
    return size() == 0;
  }

  size_t size() const noexcept {
    return offset_ ? getSize(offset_) : 0;
  }

  size_t fixed_size() const noexcept {
    return offset_ ? getSize(offset_) * sizeof(T) + getBytes(offset_) : 0;
  }

  template <class F>
  void write(size_t n, F f) {
    size_t bytes = calcBytes(n);
    if (bytes == 0) {
      throw std::overflow_error("vector::write");
    }
    uint8_t* p = reinterpret_cast<uint8_t*>(std::malloc(n * sizeof(T) + bytes));
    setSize(p, n);
    if (n > 0) {
      f(reinterpret_cast<T*>(p + bytes), n);
    }
    if (offset_) {
      uint8_t* old = offset_.get();
      offset_ = p;
      if (!getMask(old)) {
        std::free(old);
      }
    } else {
      offset_ = p;
    }
  }

  bool withBufferMask() const noexcept {
    return offset_ && getMask(offset_);
  }

  friend void serialize<>(const vector<T>& from, vector<T>& to, void* buffer);

 private:
  OffsetPtr<uint8_t> offset_;
};

} // namespace crystal
