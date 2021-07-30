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
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "crystal/serializer/record/OffsetPtr.h"

namespace crystal {

class String {
 public:
  using value_type = char;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  String() noexcept = default;

  ~String() {
    if (offset_) {
      std::free(offset_.get());
      offset_ = nullptr;
    }
  }

  String(size_t n, char c) {
    assign(n, c);
  }
  String(const String& other, size_t pos) {
    assign(other, pos, other.size() - pos);
  }
  String(const String& other, size_t pos, size_t n) {
    assign(other, pos, n);
  }
  String(const char* s, size_t n) {
    assign(s, n);
  }
  String(const char* s) {
    assign(s);
  }
  String(const String& other) {
    assign(other);
  }
  String(String&& other) {
    std::swap(offset_, other.offset_);
  }

  String& operator=(const String& other) {
    return assign(other);
  }
  String& operator=(String&& other) noexcept {
    return assign(other);
  }
  String& operator=(const char* s) {
    return assign(s);
  }

  String& assign(size_t n, char c) {
    char* p = reserve(n);
    std::memset(p, c, n);
    reset(p);
    return *this;
  }
  String& assign(const String& str) {
    size_t n = str.size();
    char* p = reserve(n);
    std::memcpy(p, str.data(), n);
    reset(p);
    return *this;
  }
  String& assign(const String& str, size_t pos, size_t n = npos) {
    if (pos <= str.size()) {
      n = std::min(n, str.size() - pos);
      char* p = reserve(n);
      std::memcpy(p, str.data() + pos, n);
      reset(p);
    }
    return *this;
  }
  String& assign(String&& str) noexcept {
    std::swap(offset_, str.offset_);
    return *this;
  }
  String& assign(const char* s, size_t n) {
    char* p = reserve(n);
    std::memcpy(p, s, n);
    reset(p);
    return *this;
  }
  String& assign(const char* s) {
    size_t n = std::strlen(s);
    char* p = reserve(n);
    std::memcpy(p, s, n);
    reset(p);
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
    return reinterpret_cast<char*>(offset_ + 1)[pos];
  }
  const_reference operator[](size_t pos) const {
    return reinterpret_cast<char*>(offset_ + 1)[pos];
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

  char* data() noexcept = delete;
  const char* data() const noexcept {
    return offset_ ? reinterpret_cast<const char*>(offset_ + 1) : nullptr;
  }

  std::string str() const {
    return std::string(data(), size());
  }
  const char* c_str() const noexcept = delete;

  operator std::string_view() const noexcept {
    return std::string_view(data(), size());
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

  char* reserve(size_t n = 0) {
    uint32_t* p = reinterpret_cast<uint32_t*>(
        std::malloc(n + sizeof(uint32_t)));
    *p = n;
    return reinterpret_cast<char*>(p + 1);
  }
  void reset(char* p) {
    this->~String();
    offset_ = reinterpret_cast<uint32_t*>(p) - 1;
  }

  static const size_t npos = -1;

 private:
  OffsetPtr<uint32_t> offset_;
};

} // namespace crystal
