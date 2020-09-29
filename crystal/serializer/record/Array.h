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

#include <string>
#include <string_view>

#include "crystal/foundation/BitMask.h"
#include "crystal/memory/Allocator.h"
#include "crystal/memory/AllocatorUtil.h"
#include "crystal/serializer/record/detail/Bitset.h"
#include "crystal/type/DataType.h"

namespace crystal {

template <class T>
class Array {
 public:
  typedef T value_type;

  Array() {}
  Array(const void* ptr,
        size_t size,
        size_t bitOffset = 0,
        size_t bits = 0,
        OffsetBitMask bitMask = OffsetBitMask())
      : ptr_(reinterpret_cast<uint8_t*>(const_cast<void*>(ptr))),
        size_(size),
        bitOffset_(bitOffset),
        bits_(bits),
        bitMask_(bitMask) {}

  virtual ~Array() {}

  T get(size_t i) const {
    if (bits_ == 0) {
      return *reinterpret_cast<T*>(ptr_ + sizeof(T) * i);
    }
    Bitset bitset(bits_);
    bitset.deserialize(ptr_, bitOffset_ + bits_ * i);
    return bitset.retrieve();
  }

  bool set(size_t i, const T& value) {
    bitMask_.set(true);
    if (bits_ == 0) {
      *reinterpret_cast<T*>(ptr_ + sizeof(T) * i) = value;
      return true;
    }
    return Bitset(bits_, value).serialize(ptr_, bitOffset_ + bits_ * i);
  }

  const T* data() const {
    return reinterpret_cast<T*>(ptr_);
  }

  size_t size() const {
    return size_;
  }

 private:
  uint8_t* ptr_;
  size_t size_;
  size_t bitOffset_ : 32;
  size_t bits_ : 32;
  OffsetBitMask bitMask_;
};

template <>
class Array<bool> {
 public:
  typedef bool value_type;

  Array() {}
  Array(const void* ptr,
        size_t size,
        size_t bitOffset = 0,
        OffsetBitMask bitMask = OffsetBitMask())
      : ptr_(reinterpret_cast<uint8_t*>(const_cast<void*>(ptr))),
        size_(size),
        bitOffset_(bitOffset),
        bitMask_(bitMask) {}

  virtual ~Array() {}

  bool get(size_t i) const {
    Bitset bitset(1);
    bitset.deserialize(ptr_, bitOffset_ + i);
    return bitset.test(0);
  }

  bool set(size_t i, const bool& value) {
    bitMask_.set(true);
    return Bitset(1, value ? 1 : 0).serialize(ptr_, bitOffset_ + i);
  }

  const uint8_t* data() const {
    return ptr_;
  }

  size_t size() const {
    return size_;
  }

 private:
  uint8_t* ptr_;
  size_t size_;
  size_t bitOffset_;
  OffsetBitMask bitMask_;
};

template <>
class Array<std::string_view> {
 public:
  typedef std::string_view value_type;

  Array() {}
  Array(const void* ptr,
        size_t size,
        Allocator* alloc,
        std::string_view dflt = "",
        OffsetBitMask bitMask = OffsetBitMask())
      : ptr_(reinterpret_cast<int64_t*>(const_cast<void*>(ptr))),
        size_(size),
        alloc_(alloc),
        default_(dflt),
        bitMask_(bitMask) {}

  virtual ~Array() {}

  std::string_view get(size_t i) const;

  bool set(size_t i, const std::string_view& value);

  const int64_t* data() const {
    return ptr_;
  }

  size_t size() const {
    return size_;
  }

  Array<std::string> toStdStringArray() const;

 private:
  int64_t* ptr_;
  size_t size_;
  Allocator* alloc_;
  std::string_view default_;
  OffsetBitMask bitMask_;
};

template <>
class Array<std::string> {
 public:
  typedef std::string value_type;

  Array() {}
  Array(const void* ptr,
        size_t size,
        Allocator* alloc,
        std::string_view dflt = "",
        OffsetBitMask bitMask = OffsetBitMask())
      : ptr_(reinterpret_cast<int64_t*>(const_cast<void*>(ptr))),
        size_(size),
        alloc_(alloc),
        default_(dflt),
        bitMask_(bitMask) {}

  virtual ~Array() {}

  std::string get(size_t i) const;

  bool set(size_t i, const std::string& value);

  const int64_t* data() const {
    return ptr_;
  }

  size_t size() const {
    return size_;
  }

 private:
  int64_t* ptr_;
  size_t size_;
  Allocator* alloc_;
  std::string_view default_;
  OffsetBitMask bitMask_;
};


template <class T> struct IsArray : std::false_type {};

template <> struct IsArray<Array<bool>> : std::true_type {};
template <> struct IsArray<Array<int8_t>> : std::true_type {};
template <> struct IsArray<Array<int16_t>> : std::true_type {};
template <> struct IsArray<Array<int32_t>> : std::true_type {};
template <> struct IsArray<Array<int64_t>> : std::true_type {};
template <> struct IsArray<Array<uint8_t>> : std::true_type {};
template <> struct IsArray<Array<uint16_t>> : std::true_type {};
template <> struct IsArray<Array<uint32_t>> : std::true_type {};
template <> struct IsArray<Array<uint64_t>> : std::true_type {};
template <> struct IsArray<Array<float>> : std::true_type {};
template <> struct IsArray<Array<double>> : std::true_type {};
template <> struct IsArray<Array<std::string>> : std::true_type {};
template <> struct IsArray<Array<std::string_view>> : std::true_type {};

//////////////////////////////////////////////////////////////////////

inline std::string_view Array<std::string_view>::get(size_t i) const {
  int64_t offset = ptr_[i];
  if (offset == 0) {
    return default_;
  }
  auto p = readBufferAndSize(alloc_, offset);
  return std::string_view(reinterpret_cast<char*>(p.first), p.second);
}

inline std::string Array<std::string>::get(size_t i) const {
  int64_t offset = ptr_[i];
  if (offset == 0) {
    return std::string(default_);
  }
  auto p = readBufferAndSize(alloc_, offset);
  return std::string(reinterpret_cast<char*>(p.first), p.second);
}

}  // namespace crystal
