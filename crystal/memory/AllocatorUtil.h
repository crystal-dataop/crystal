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

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include "crystal/memory/Memory.h"

namespace crystal {

template <class T, class Alloc>
inline T* address(const Alloc* alloc, int64_t offset) {
  return reinterpret_cast<T*>(alloc->address(offset));
}

template <class T, class Alloc>
int64_t write(Alloc* alloc, const T& value, size_t n = 1) {
  int64_t offset = alloc->allocate(sizeof(T) * n);
  if (offset > 0) {
    T* ptr = address<T>(alloc, offset);
    for (size_t i = 0; i < n; ++i) {
      ptr[i] = value;
    }
  }
  return offset;
}

template <class Alloc>
inline uint32_t getBufferSize(const Alloc* alloc, int64_t offset) {
  return *reinterpret_cast<uint32_t*>(alloc->address(offset));
}

template <class Alloc>
inline void* readBuffer(const Alloc* alloc, int64_t offset) {
  return reinterpret_cast<uint32_t*>(alloc->address(offset)) + 1;
}

template <class Alloc>
inline std::pair<void*, uint32_t>
readBufferAndSize(const Alloc* alloc, int64_t offset) {
  void* ptr = alloc->address(offset);
  uint32_t* p = reinterpret_cast<uint32_t*>(ptr);
  return std::make_pair(p + 1, *p);
}

template <class Alloc>
int64_t allocBuffer(Alloc* alloc, uint32_t size, bool init = false) {
  int64_t offset = alloc->allocate(size + sizeof(uint32_t));
  if (offset > 0) {
    *address<uint32_t>(alloc, offset) = size;
    if (init && size > 0) {
      memset(address<uint8_t>(alloc, offset) + sizeof(uint32_t), 0, size);
    }
  }
  return offset;
}

template <class Alloc>
int64_t writeBuffer(Alloc* alloc, const void* data, uint32_t size) {
  int64_t offset = allocBuffer(alloc, size);
  if (offset > 0 && size > 0) {
    memcpy(address<uint8_t>(alloc, offset) + sizeof(uint32_t), data, size);
  }
  return offset;
}

}  // namespace crystal
