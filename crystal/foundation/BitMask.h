/*
 * Copyright 2020 Yeolar
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
#include <cstring>

namespace crystal {

class BitMask {
 public:
  explicit BitMask(uint8_t* ptr = nullptr) : ptr_(ptr) {}

  void reset(uint8_t* ptr);

  void clear(size_t n);

  bool isSet(int64_t offset) const;

  void set(int64_t offset);
  void unset(int64_t offset);

 private:
  static const uint8_t sU8Mask[8];

  uint8_t* ptr_;
};

struct OffsetBitMask {
  uint8_t* ptr{nullptr};
  int64_t offset;

  OffsetBitMask() {}
  OffsetBitMask(uint8_t* p, int64_t o)
      : ptr(p), offset(o) {}

  bool isSet() const;
  void set(bool masked);
};

//////////////////////////////////////////////////////////////////////

inline void BitMask::reset(uint8_t* ptr) {
  ptr_ = ptr;
}

inline void BitMask::clear(size_t n) {
  memset(ptr_, 0, n);
}

inline bool BitMask::isSet(int64_t offset) const {
  return ptr_[offset / 8] & sU8Mask[offset % 8];
}

inline void BitMask::set(int64_t offset) {
  ptr_[offset / 8] |= sU8Mask[offset % 8];
}

inline void BitMask::unset(int64_t offset) {
  ptr_[offset / 8] &= ~sU8Mask[offset % 8];
}

inline bool OffsetBitMask::isSet() const {
  return ptr ? BitMask(ptr).isSet(offset) : false;
}

inline void OffsetBitMask::set(bool masked) {
  if (ptr) {
    if (masked) {
      BitMask(ptr).set(offset);
    } else {
      BitMask(ptr).unset(offset);
    }
  }
}

} // namespace crystal
