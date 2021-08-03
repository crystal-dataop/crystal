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

namespace crystal {

template <class T>
constexpr size_t kNoMaskBitCount = sizeof(T) * 8 - 1;

struct Head {
  uint8_t mask : 1;
  uint8_t nvar : 3;
  size_t _size : 4;
};

inline bool getMask(uint8_t* p) {
  return reinterpret_cast<Head*>(p)->mask;
}

inline size_t getBytes(uint8_t* p) {
  return reinterpret_cast<Head*>(p)->nvar + 1;
}

inline size_t getSize(uint8_t* p) {
  Head* h = reinterpret_cast<Head*>(p);
  switch (h->nvar) {
    case 0: return *p & 0xf;
    case 1: return *reinterpret_cast<uint16_t*>(p) & 0xfff;
    case 3: return *reinterpret_cast<uint32_t*>(p) & 0xfffffff;
    case 7: return *reinterpret_cast<uint64_t*>(p) & 0xfffffffffffffff;
  }
  return 0;
}

inline size_t calcBytes(size_t n) {
  if (n <= 0xf) return 1;
  if (n <= 0xfff) return 2;
  if (n <= 0xfffffff) return 4;
  if (n <= 0xfffffffffffffff) return 8;
  return 0;
}

inline void setMask(uint8_t* p) {
  reinterpret_cast<Head*>(p)->mask = 1;
}

inline void setSize(uint8_t* p, size_t n) {
  Head* h = reinterpret_cast<Head*>(p);
  switch (calcBytes(n)) {
    case 0: break;
    case 1: *p = n; h->nvar = 0; break;
    case 2: *reinterpret_cast<uint16_t*>(p) = n; h->nvar = 1; break;
    case 4: *reinterpret_cast<uint32_t*>(p) = n; h->nvar = 3; break;
    case 8: *reinterpret_cast<uint64_t*>(p) = n; h->nvar = 7; break;
  }
}

} // namespace crystal
