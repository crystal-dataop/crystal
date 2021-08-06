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

inline size_t bufferSizeToUpdate(const bool&) {
  return 0;
}

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

inline void syncOffset(bool&, bool&) {
}
inline void serializeInUpdating(bool&, void*) {
}

template <class T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline void syncOffset(T&, T&) {
}
template <class T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline void serializeInUpdating(T&, void*) {
}

inline void syncOffset(string& from, string& to) {
  to.offset_ = from.offset_;
  from.offset_ = nullptr;
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
inline void syncOffset(pair<T1, T2>& from, pair<T1, T2>& to) {
  syncOffset(from.first, to.first);
  syncOffset(from.second, to.second);
}
template <class T1, class T2>
inline void serializeInUpdating(pair<T1, T2>& value, void* buffer) {
  uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
  serializeInUpdating(value.first, p);
  serializeInUpdating(value.second, p + bufferSizeToUpdate(value.first));
}

template <class T, size_t N>
void syncOffset(array<T, N>& from, array<T, N>& to) {
  for (size_t i = 0; i < N; ++i) {
    syncOffset(from[i], to[i]);
  }
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
inline void syncOffset(vector<T>& from, vector<T>& to) {
  to.offset_ = from.offset_;
}
template <class T>
void serializeInUpdating(vector<T>& value, void* buffer) {
  if (!value.with_buffer_mask()) {
    uint8_t* old = value.offset_.get();
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
    size_t n = value.fixed_size();
    std::memcpy(buf, old, n);
    setMask(buf);
    uint8_t* p = buf;
    p += n;
    size_t size = value.size();
    size_t bytes = calcBytes(size);
    for (size_t i = 0; i < size; ++i) {
      T& from = reinterpret_cast<T*>(old + bytes)[i];
      T& to = reinterpret_cast<T*>(buf + bytes)[i];
      n = bufferSizeToUpdate(from);
      syncOffset(from, to);
      serializeInUpdating(to, p);
      p += n;
    }
    value.set_buffer(buf);
  } else {
    uint8_t* old = value.offset_.get();
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    size_t n = 0;
    size_t size = value.size();
    size_t bytes = calcBytes(size);
    for (size_t i = 0; i < size; ++i) {
      T& from = reinterpret_cast<T*>(old + bytes)[i];
      n = bufferSizeToUpdate(from);
      serializeInUpdating(from, p);
      p += n;
    }
  }
}

inline void syncOffset(untyped_tuple& from, untyped_tuple& to) {
  to.offset_ = from.offset_;
}
void serializeInUpdating(untyped_tuple::meta& value, void* buffer);
void serializeInUpdating(untyped_tuple& value, void* buffer);

} // namespace crystal
