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

#include <cassert>
#include <cstdint>
#include <cstring>
#include <utility>

namespace crystal {

template <class T>
class NumericIndexArray {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef T* iterator;
  typedef T* const const_iterator;

  NumericIndexArray() {}
  NumericIndexArray(size_t capacity);

  virtual ~NumericIndexArray();

  NumericIndexArray(const NumericIndexArray&) = delete;
  NumericIndexArray& operator=(const NumericIndexArray&) = delete;

  NumericIndexArray(NumericIndexArray&& other);
  NumericIndexArray& operator=(NumericIndexArray&& other);

  void reserve(size_t size);
  void resize(size_t size, bool fillZero = false);

  void clear();

  T* data() const;

  size_t size() const;
  size_t capacity() const;

  bool empty() const;

  T* begin();
  const T* begin() const;
  T* end();
  const T* end() const;

  T front() const;
  T back() const;

  T operator[](int i) const;

  void push_back(T value);
  void pop_back();

  void swap(NumericIndexArray& other);

  bool merge(NumericIndexArray& other, T offset = 0);

 private:
  size_t size_{0};
  size_t capacity_{0};
  pointer ptr_{nullptr};
};

typedef NumericIndexArray<uint32_t> U32IndexArray;
typedef NumericIndexArray<uint64_t> U64IndexArray;

}  // namespace crystal

#include "crystal/dataframe/NumericIndexArray-inl.h"
