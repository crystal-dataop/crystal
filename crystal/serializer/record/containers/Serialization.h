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
#include <type_traits>

#include "crystal/serializer/record/containers/Array.h"
#include "crystal/serializer/record/containers/Pair.h"
#include "crystal/serializer/record/containers/String.h"
#include "crystal/serializer/record/containers/UnTypedTuple.h"
#include "crystal/serializer/record/containers/Vector.h"

namespace crystal {

inline size_t bufferSize(const bool&) {
  return 0;
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline size_t bufferSize(const T&) {
  return 0;
}

inline size_t bufferSize(const string& value) {
  size_t n = value.size();
  return n + calcBytes(n);
}

template <class T1, class T2>
inline size_t bufferSize(const pair<T1, T2>& value) {
  return bufferSize(value.first) + bufferSize(value.second);
}

template <class T, size_t N>
size_t bufferSize(const array<T, N>& value) {
  size_t n = 0;
  for (auto& element : value) {
    n += bufferSize(element);
  }
  return n;
}

template <class T>
size_t bufferSize(const vector<T>& value) {
  size_t n = value.fixed_size();
  for (auto& element : value) {
    n += bufferSize(element);
  }
  return n;
}

size_t bufferSize(const untyped_tuple::meta& value);
size_t bufferSize(const untyped_tuple& value);

inline void serialize(const bool&, bool&, void*) {
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline void serialize(const T&, T&, void*) {
}

inline void serialize(const string& from, string& to, void* buffer) {
  uint8_t* old = from.offset_.get();
  uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
  std::memcpy(buf, old, bufferSize(from));
  setMask(buf);
  to.offset_ = buf;
}

template <class T1, class T2>
inline
void serialize(const pair<T1, T2>& from, pair<T1, T2>& to, void* buffer) {
  serialize(from.first, to.first, buffer);
  serialize(from.second, to.second, buffer + bufferSize(from.first));
}

template <class T, size_t N>
void serialize(const array<T, N>& from, array<T, N>& to, void* buffer) {
  for (size_t i = 0; i < N; ++i) {
    serialize(from[i], to[i], buffer);
    buffer += bufferSize(from[i]);
  }
}

template <class T>
void serialize(const vector<T>& from, vector<T>& to, void* buffer) {
  uint8_t* old = from.offset_.get();
  uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
  size_t n = from.fixed_size();
  std::memcpy(buf, old, n);
  setMask(buf);
  uint8_t* p = buf;
  p += n;
  size_t size = from.size();
  size_t bytes = calcBytes(size);
  for (size_t i = 0; i < size; ++i) {
    serialize(reinterpret_cast<T*>(old + bytes)[i],
              reinterpret_cast<T*>(buf + bytes)[i],
              p);
    p += bufferSize(reinterpret_cast<T*>(old + bytes)[i]);
  }
  to.offset_ = buf;
}

void serialize(const untyped_tuple::meta& from,
               untyped_tuple::meta& to,
               void* buffer);
void serialize(const untyped_tuple& from, untyped_tuple& to, void* buffer);

} // namespace crystal
