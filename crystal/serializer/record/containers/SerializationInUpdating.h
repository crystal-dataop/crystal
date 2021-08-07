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
#include <type_traits>

#include "crystal/serializer/record/containers/Array.h"
#include "crystal/serializer/record/containers/Pair.h"
#include "crystal/serializer/record/containers/String.h"
#include "crystal/serializer/record/containers/UnTypedTuple.h"
#include "crystal/serializer/record/containers/Vector.h"

namespace crystal {

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline size_t bufferSizeToUpdate(const T&) {
  return 0;
}

inline size_t bufferSizeToUpdate(const string& value) {
  size_t n = value.size();
  return value.with_buffer_mask() ? 0 : n + calcBytes(n);
}

template <class T1, class T2>
inline size_t bufferSizeToUpdate(const pair<T1, T2>& value) {
  return bufferSizeToUpdate(value.first) + bufferSizeToUpdate(value.second);
}

template <class T, size_t N>
size_t bufferSizeToUpdate(const array<T, N>& value) {
  size_t n = 0;
  for (auto& element : value) {
    n += bufferSizeToUpdate(element);
  }
  return n;
}

template <class T>
size_t bufferSizeToUpdate(const vector<T>& value) {
  size_t n = value.with_buffer_mask() ? 0 : value.fixed_size();
  for (auto& element : value) {
    n += bufferSizeToUpdate(element);
  }
  return n;
}

size_t bufferSizeToUpdate(const untyped_tuple::meta& value);
size_t bufferSizeToUpdate(const untyped_tuple& value);

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline void serializeInUpdating(T&, void*) {
}

inline void serializeInUpdating(string& value, void* buffer) {
  if (!value.with_buffer_mask()) {
    uint8_t* old = value.offset_.get();
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
    std::memcpy(buf, old, bufferSizeToUpdate(value));
    setMask(buf);
    value.set_buffer(buf);
  }
}

template <class T1, class T2>
inline void serializeInUpdating(pair<T1, T2>& value, void* buffer) {
  uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
  serializeInUpdating(value.first, p);
  serializeInUpdating(value.second, p + bufferSizeToUpdate(value.first));
}

template <class T, size_t N>
void serializeInUpdating(array<T, N>& value, void* buffer) {
  uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
  for (size_t i = 0; i < N; ++i) {
    serializeInUpdating(value[i], p);
    p += bufferSizeToUpdate(value[i]);
  }
}

template <class T>
void serializeInUpdating(vector<T>& value, void* buffer) {
  if (!value.with_buffer_mask()) {
    uint8_t* old = value.offset_.get();
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
    size_t size = value.size();
    size_t bytes = calcBytes(size);
    std::memcpy(buf, old, bytes);
    setMask(buf);
    uint8_t* p = buf + value.fixed_size();
    for (size_t i = 0; i < size; ++i) {
      T& from = reinterpret_cast<T*>(old + bytes)[i];
      T& to = reinterpret_cast<T*>(buf + bytes)[i];
      size_t n = bufferSizeToUpdate(from);
      serializeInUpdating(from, p);
      new (&to) T();
      new (&to) T(std::move(from));
      p += n;
    }
    value.set_buffer(buf);
  } else {
    uint8_t* old = value.offset_.get();
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    size_t size = value.size();
    size_t bytes = calcBytes(size);
    for (size_t i = 0; i < size; ++i) {
      T& from = reinterpret_cast<T*>(old + bytes)[i];
      size_t n = bufferSizeToUpdate(from);
      serializeInUpdating(from, p);
      p += n;
    }
  }
}

void serializeInUpdating(untyped_tuple::meta& value, void* buffer);
void serializeInUpdating(untyped_tuple& value, void* buffer);

} // namespace crystal
