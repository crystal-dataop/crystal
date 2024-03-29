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
#include <cstring>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "crystal/record/AllocMask.h"
#include "crystal/record/OffsetPtr.h"
#include "crystal/type/DataType.h"

namespace crystal {

class string {
 public:
  using value_type = char;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  string() noexcept = default;

  ~string() {
    set_buffer(nullptr);
  }

  string(size_t n, char c) {
    assign(n, c);
  }
  string(const string& other, size_t pos) {
    assign(other, pos, other.size() - pos);
  }
  string(const string& other, size_t pos, size_t n) {
    assign(other, pos, n);
  }
  string(const char* s, size_t n) {
    assign(s, n);
  }
  string(const char* s) {
    assign(s);
  }
  template <class It>
  string(It first, It last) {
    assign(first, last);
  }
  string(const string& other) {
    assign(other);
  }
  string(string&& other) {
    std::swap(offset_, other.offset_);
  }
  string(std::initializer_list<char> list) {
    assign(list);
  }

  string& operator=(const string& other) {
    return assign(other);
  }
  string& operator=(string&& other) noexcept {
    return assign(other);
  }
  string& operator=(const char* s) {
    return assign(s);
  }
  string& operator=(std::initializer_list<char> list) {
    return assign(list);
  }

  string& assign(size_t n, char c) {
    write(n, [&](char* p, size_t n) { std::memset(p, c, n); });
    return *this;
  }
  string& assign(const string& str) {
    size_t n = str.size();
    write(n, [&](char* p, size_t n) { std::memcpy(p, str.data(), n); });
    return *this;
  }
  string& assign(const string& str, size_t pos, size_t n = npos) {
    if (pos <= str.size()) {
      n = std::min(n, str.size() - pos);
      write(n, [&](char* p, size_t n) { std::memcpy(p, str.data() + pos, n); });
    }
    return *this;
  }
  string& assign(string&& str) noexcept {
    std::swap(offset_, str.offset_);
    return *this;
  }
  string& assign(const char* s, size_t n) {
    write(n, [&](char* p, size_t n) { std::memcpy(p, s, n); });
    return *this;
  }
  string& assign(const char* s) {
    size_t n = std::strlen(s);
    write(n, [&](char* p, size_t n) { std::memcpy(p, s, n); });
    return *this;
  }
  template <class InputIt>
  string& assign(InputIt first, InputIt last) {
    size_t n = last - first;
    write(n, [&](char* p, size_t) {
      for (char* it = p; first != last; ) {
        *it++ = *first++;
      }
    });
    return *this;
  }
  string& assign(std::initializer_list<char> list) {
    size_t n = list.size();
    write(n, [&](char* p, size_t) {
      char* it = p;
      for (const char& c : list) {
        *it++ = c;
      }
    });
    return *this;
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
    return reinterpret_cast<char*>(offset_ + getBytes(offset_))[pos];
  }
  const_reference operator[](size_t pos) const {
    return reinterpret_cast<const char*>(offset_ + getBytes(offset_))[pos];
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

  char* data() noexcept {
    return offset_ ? reinterpret_cast<char*>(offset_ + getBytes(offset_))
                   : nullptr;
  }
  const char* data() const noexcept {
    return offset_ ? reinterpret_cast<const char*>(offset_ + getBytes(offset_))
                   : nullptr;
  }

  std::string str() const {
    return std::string(data(), size());
  }
  const char* c_str() const noexcept = delete;

  operator std::string_view() const noexcept {
    return std::string_view(data(), size());
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
  size_t length() const noexcept {
    return size();
  }

  template <class F>
  void write(size_t n, F f) {
    size_t bytes = calcBytes(n);
    if (bytes == 0) {
      throw std::overflow_error("string::write");
    }
    uint8_t* p = reinterpret_cast<uint8_t*>(std::malloc(n + bytes));
    setMask(p, false);
    setSize(p, n);
    if (n > 0) {
      f(reinterpret_cast<char*>(p + bytes), n);
    }
    set_buffer(p);
  }

  void set_buffer(void* buffer) {
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
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

  bool with_buffer_mask() const noexcept {
    return offset_ && getMask(offset_);
  }

  friend void serialize(const string& from, string& to, void* buffer);
  friend void serializeInUpdating(string& value, void* buffer);

  static const size_t npos = -1;

 private:
  OffsetPtr<uint8_t> offset_;
};

template <>
struct DataTypeTraits<string> {
  enum {
    value = static_cast<int>(DataType::STRING)
  };
};

} // namespace crystal
