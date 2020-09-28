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
#include <string>

#include "crystal/foundation/String.h"
#include "crystal/serializer/record/detail/MaskMap.h"

namespace crystal {

class Bitset {
 public:
  Bitset() {}

  explicit Bitset(size_t count);
  Bitset(size_t count, uint64_t value);

  virtual ~Bitset() {}

  void assign(uint64_t value);

  uint64_t retrieve() const;
  size_t count() const;

  bool test(size_t pos) const;
  bool set(size_t pos);
  bool unset(size_t pos);

  struct Mask {
    int64_t shift{0};
    uint64_t mask1{0};
    uint64_t mask2{0};
  };

  bool generateMask(int64_t offset, Mask& mask) const;

  bool serialize(uint8_t* buf, int64_t offset) const;
  bool serialize(uint8_t* buf, int64_t offset, const Mask& mask) const;

  bool deserialize(const uint8_t* buf, int64_t offset);
  bool deserialize(const uint8_t* buf, int64_t offset, const Mask& mask);

  std::string toString() const;

 private:
  static MaskMap maskMap_;

  uint64_t value_{0};
  size_t count_{MaskMap::N};
};

bool operator==(const Bitset& lhs, const Bitset& rhs);
bool operator!=(const Bitset& lhs, const Bitset& rhs);

//////////////////////////////////////////////////////////////////////

inline Bitset::Bitset(size_t count) {
  count_ = count <= MaskMap::N ? count : 0;
}

inline Bitset::Bitset(size_t count, uint64_t value) : Bitset(count) {
  assign(value);
}

inline void Bitset::assign(uint64_t value) {
  if (count_ != 0) {
    value_ = maskMap_.lowBitset(value, count_);
  }
}

inline uint64_t Bitset::retrieve() const {
  return value_;
}

inline size_t Bitset::count() const {
  return count_;
}

inline bool Bitset::test(size_t pos) const {
  if (count_ == 0 || pos > MaskMap::P) {
    return false;
  }
  return maskMap_.test(value_, pos) != 0;
}

inline bool Bitset::set(size_t pos) {
  if (count_ == 0 || pos > MaskMap::P) {
    return false;
  }
  value_ = maskMap_.set(value_, pos);
  return true;
}

inline bool Bitset::unset(size_t pos) {
  if (count_ == 0 || pos > MaskMap::P) {
    return false;
  }
  value_ = maskMap_.unset(value_, pos);
  return true;
}

inline bool Bitset::generateMask(int64_t offset, Mask& mask) const {
  if (count_ == 0 || offset < 0) {
    return false;
  }
  int64_t bitOffset = offset % MaskMap::N;
  mask.shift = int64_t(MaskMap::N - count_) - bitOffset;
  int64_t shift = std::min(mask.shift, int64_t(0));
  mask.mask1 = maskMap_.mask(bitOffset, count_ + shift);
  if (shift < 0) {
    mask.mask2 = maskMap_.mask(0, -shift);
  }
  return true;
}

inline bool Bitset::serialize(uint8_t* buf, int64_t offset) const {
  Mask mask;
  if (!generateMask(offset, mask)) {
    return false;
  }
  return serialize(buf, offset, mask);
}

inline bool Bitset::serialize(
    uint8_t* buf, int64_t offset, const Mask& mask) const {
  if (count_ == 0 || offset < 0) {
    return false;
  }
  int64_t byteOffset = offset / MaskMap::N;
  uint64_t* out = reinterpret_cast<uint64_t*>(buf);
  if (mask.shift >= 0) {
    uint64_t& a = out[byteOffset];
    a = (a & ~mask.mask1) | (value_ << mask.shift & mask.mask1);
  } else {
    uint64_t& a = out[byteOffset];
    uint64_t& b = out[byteOffset + 1];
    a = (a & ~mask.mask1) | (value_ >> -mask.shift & mask.mask1);
    b = (b & ~mask.mask2) | (value_ << (MaskMap::N + mask.shift) & mask.mask2);
  }
  return true;
}

inline bool Bitset::deserialize(const uint8_t* buf, int64_t offset) {
  Mask mask;
  if (!generateMask(offset, mask)) {
    return false;
  }
  return deserialize(buf, offset, mask);
}

inline bool Bitset::deserialize(
    const uint8_t* buf, int64_t offset, const Mask& mask) {
  if (count_ == 0 || offset < 0) {
    return false;
  }
  int64_t byteOffset = offset / MaskMap::N;
  const uint64_t* in = reinterpret_cast<const uint64_t*>(buf);
  if (mask.shift >= 0) {
    const uint64_t& a = in[byteOffset];
    value_ = (a & mask.mask1) >> mask.shift;
  } else {
    const uint64_t& a = in[byteOffset];
    const uint64_t& b = in[byteOffset + 1];
    value_ = (a & mask.mask1) << -mask.shift |
             (b & mask.mask2) >> (MaskMap::N + mask.shift);
  }
  return true;
}

inline std::string Bitset::toString() const {
  return stringPrintf("0x%llx:%zu", value_, count_);
}

inline bool operator==(const Bitset& lhs, const Bitset& rhs) {
  return lhs.retrieve() == rhs.retrieve();
}

inline bool operator!=(const Bitset& lhs, const Bitset& rhs) {
  return lhs.retrieve() != rhs.retrieve();
}

}  // namespace crystal
