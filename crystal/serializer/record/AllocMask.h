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

template <class T>
inline T mask(T bits) {
  return bits >> kNoMaskBitCount<T>;
}

template <class T>
inline void maskValue(T& bits) {
  bits |= (1 << kNoMaskBitCount<T>);
}

template <class T>
inline T unmaskValue(T bits) {
  return bits & ~(1 << kNoMaskBitCount<T>);
}

struct Head {
  uint8_t mask : 1;
  uint8_t nvar : 3;
  size_t _size : 4;
};

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

} // namespace crystal
