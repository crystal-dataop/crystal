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
#include <cstddef>
#include <cstdint>

namespace crystal {

struct MaskMap {
 public:
  static constexpr size_t N = 64;
  static constexpr size_t P = 63;

  MaskMap();

  uint64_t test(uint64_t value, size_t pos) const;
  uint64_t set(uint64_t value, size_t pos) const;
  uint64_t unset(uint64_t value, size_t pos) const;
  uint64_t lowBitset(uint64_t value, size_t count) const;
  uint64_t mask(int64_t offset, size_t count) const;

 private:
  uint64_t get(size_t i, size_t j) const {
    return array_[i][j];
  }

  /*
   * 1000.. 1100.. 1110.. 1111..
   * 0000.. 0100.. 0110.. 0111..
   * 0000.. 0000.. 0010.. 0011..
   * 0000.. 0000.. 0000.. 0001..
   * ..
   */
  uint64_t array_[N][N];
};

//////////////////////////////////////////////////////////////////////

inline uint64_t MaskMap::test(uint64_t value, size_t pos) const {
  assert(pos <= P);
  return value & get(P - pos, P - pos);
}

inline uint64_t MaskMap::set(uint64_t value, size_t pos) const {
  assert(pos <= P);
  return value | get(P - pos, P - pos);
}

inline uint64_t MaskMap::unset(uint64_t value, size_t pos) const {
  assert(pos <= P);
  return value & ~get(P - pos, P - pos);
}

inline uint64_t MaskMap::lowBitset(uint64_t value, size_t count) const {
  assert(count <= N);
  return count ? value & get(N - count, P) : 0;
}

inline uint64_t MaskMap::mask(int64_t offset, size_t count) const {
  assert(offset + count <= N);
  return get(offset, offset + count - 1);
}

}  // namespace crystal
