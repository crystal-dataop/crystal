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

#include <string>
#include <string_view>
#include <vector>

#include "crystal/serializer/record/Array.h"
#include "crystal/serializer/record/FieldBlock.h"
#include "crystal/serializer/record/RecordMeta.h"

namespace crystal {

class Accessor {
 public:
  explicit Accessor(const RecordMeta& recordMeta);
  virtual ~Accessor() {}

  template <class T>
  T getNumeric(
      const void* buf,
      const Allocator*,
      const FieldMeta& meta) const;
  bool getBool(
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) const;
  std::string_view getString(
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) const;

  template <class T>
  T get(
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) const {
    return GetImpl<T>::get(this, buf, alloc, meta);
  }

  template <class T>
  bool setNumeric(
      void* buf,
      Allocator*,
      const FieldMeta& meta,
      const T& value) const;
  bool setBool(
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const bool& value) const;
  bool setString(
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const std::string_view& value) const;

  template <class T>
  bool set(
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const T& value) const {
    return SetImpl<T>::set(this, buf, alloc, meta, value);
  }

  template <class T>
  Array<T> mgetNumeric(
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) const;
  Array<bool> mgetBool(
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) const;
  Array<std::string_view> mgetString(
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) const;

  template <class T>
  Array<T> mget(
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) const {
    return MGetImpl<T>::mget(this, buf, alloc, meta);
  }

  size_t bitOffset() const;
  size_t bufferSize() const;

  bool hasField(const void* buf, const FieldMeta& meta) const;

  bool reset(void* buf, Allocator* alloc, const RecordMeta& recordMeta) const;

  bool merge(
      void* buf, Allocator* alloc,
      const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
      const RecordMeta& recordMeta) const;

  bool copy(
      void* buf, Allocator* alloc,
      const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
      const RecordMeta& recordMeta) const;

  bool buildVarArray(
      void* buf, Allocator* alloc, const FieldMeta& meta, size_t size) const;

  dynamic toDynamic() const;
  std::string toString() const;

 private:
  template <class T>
  struct GetImpl;
  template <class T>
  struct SetImpl;
  template <class T>
  struct MGetImpl;

  void setHasField(void* buf, const FieldMeta& meta, bool set) const;

  const FieldBlock& getFieldBlock(const FieldMeta& meta) const;

  bool resetOne(void* buf, Allocator* alloc, const FieldMeta& meta) const;
  bool resetArray(void* buf, Allocator* alloc, const FieldMeta& meta) const;

  bool mergeOne(
      void* buf, Allocator* alloc,
      const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
      const FieldMeta& meta) const;
  bool mergeArray(
      void* buf, Allocator* alloc,
      const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
      const FieldMeta& meta) const;

  template <class T>
  bool mergeArrayImpl(
      void* buf, Allocator* alloc,
      const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
      const FieldMeta& meta) const;

  std::vector<FieldBlock> blocks_;
  size_t bitOffset_{0};
  size_t hasOffset_{0};
  size_t bufferSize_{0};
};

//////////////////////////////////////////////////////////////////////

template <class T>
inline T Accessor::getNumeric(
    const void* buf, const Allocator*, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buf);
  if (!meta.isCompact()) {
    return *reinterpret_cast<const T*>(ptr + block.byteOffset);
  }
  Bitset bits(meta.bits());
  bits.deserialize(ptr + bitOffset_, block.bitOffset, block.mask);
  return bits.retrieve();
}

inline bool Accessor::getBool(
    const void* buf, const Allocator*, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buf);
  Bitset bits(1);
  bits.deserialize(ptr + bitOffset_, block.bitOffset, block.mask);
  return bits.test(0);
}

inline std::string_view Accessor::getString(
    const void* buf, const Allocator* alloc, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buf);
  int64_t offset = *reinterpret_cast<const int64_t*>(ptr + block.byteOffset);
  if (offset == 0) {
    return meta.dflt<std::string_view>();
  }
  auto p = readBufferAndSize(alloc, offset);
  return std::string_view(reinterpret_cast<char*>(p.first), p.second);
}

template <class T>
struct Accessor::GetImpl {
  static T get(
      const Accessor* accessor,
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->getNumeric<T>(buf, alloc, meta);
  }
};

template <>
struct Accessor::GetImpl<bool> {
  static bool get(
      const Accessor* accessor,
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->getBool(buf, alloc, meta);
  }
};

template <>
struct Accessor::GetImpl<std::string_view> {
  static std::string_view get(
      const Accessor* accessor,
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->getString(buf, alloc, meta);
  }
};

template <>
struct Accessor::GetImpl<std::string> {
  static std::string get(
      const Accessor* accessor,
      const void* buf,
      const Allocator* alloc,
      const FieldMeta& meta) {
    return std::string(accessor->getString(buf, alloc, meta));
  }
};

template <class T>
inline bool Accessor::setNumeric(
    void* buf, Allocator*, const FieldMeta& meta, const T& value) const {
  const FieldBlock& block = getFieldBlock(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(buf);
  if (!meta.isCompact()) {
    *reinterpret_cast<T*>(ptr + block.byteOffset) = value;
  } else {
    Bitset bits(meta.bits(), value);
    bits.serialize(ptr + bitOffset_, block.bitOffset, block.mask);
  }
  setHasField(buf, meta, true);
  return true;
}

inline bool Accessor::setBool(
    void* buf, Allocator*, const FieldMeta& meta, const bool& value) const {
  const FieldBlock& block = getFieldBlock(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(buf);
  Bitset bits(1, value ? 1 : 0);
  bits.serialize(ptr + bitOffset_, block.bitOffset, block.mask);
  setHasField(buf, meta, true);
  return true;
}

inline bool Accessor::setString(
    void* buf, Allocator* alloc, const FieldMeta& meta,
    const std::string_view& value) const {
  const FieldBlock& block = getFieldBlock(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(buf);
  int64_t& offset = *reinterpret_cast<int64_t*>(ptr + block.byteOffset);
  if (offset != 0) {
    alloc->deallocate(offset);
    offset = 0;
  }
  offset = writeBuffer(alloc, value.data(), value.size());
  if (offset == 0) {
    return false;
  }
  setHasField(buf, meta, true);
  return true;
}

template <class T>
struct Accessor::SetImpl {
  static bool set(
      const Accessor* accessor,
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const T& value) {
    return accessor->setNumeric<T>(buf, alloc, meta, value);
  }
};

template <>
struct Accessor::SetImpl<bool> {
  static bool set(
      const Accessor* accessor,
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const bool& value) {
    return accessor->setBool(buf, alloc, meta, value);
  }
};

template <>
struct Accessor::SetImpl<std::string_view> {
  static bool set(
      const Accessor* accessor,
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const std::string_view& value) {
    return accessor->setString(buf, alloc, meta, value);
  }
};

template <>
struct Accessor::SetImpl<std::string> {
  static bool set(
      const Accessor* accessor,
      void* buf,
      Allocator* alloc,
      const FieldMeta& meta,
      const std::string& value) {
    return accessor->setString(buf, alloc, meta, value);
  }
};

template <class T>
inline Array<T> Accessor::mgetNumeric(
    const void* buf, Allocator* alloc, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(buf));
  if (meta.isFixArray()) {
    return Array<T>(
        meta.isCompact() ? ptr + bitOffset_ : ptr + block.byteOffset,
        meta.count(),
        meta.isCompact() ? block.bitOffset : 0,
        meta.isCompact() ? block.itemBitSize : 0,
        OffsetBitMask(ptr + hasOffset_, meta.tag()));
  }
  int64_t offset = *reinterpret_cast<int64_t*>(ptr + block.byteOffset);
  if (offset == 0) {
    return Array<T>(nullptr, 0);
  }
  auto p = readBufferAndSize(alloc, offset);
  if (p.second == 0) {
    return Array<T>(nullptr, 0);
  }
  return Array<T>(
      p.first,
      p.second,
      0,
      meta.isCompact() ? block.itemBitSize : 0,
      OffsetBitMask(ptr + hasOffset_, meta.tag()));
}

inline Array<bool> Accessor::mgetBool(
    const void* buf, Allocator* alloc, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(buf));
  if (meta.isFixArray()) {
    return Array<bool>(
        ptr + bitOffset_,
        meta.count(),
        block.bitOffset,
        OffsetBitMask(ptr + hasOffset_, meta.tag()));
  }
  int64_t offset = *reinterpret_cast<int64_t*>(ptr + block.byteOffset);
  if (offset == 0) {
    return Array<bool>(nullptr, 0);
  }
  auto p = readBufferAndSize(alloc, offset);
  if (p.second == 0) {
    return Array<bool>(nullptr, 0);
  }
  return Array<bool>(
      p.first,
      p.second,
      0,
      OffsetBitMask(ptr + hasOffset_, meta.tag()));
}

inline Array<std::string_view> Accessor::mgetString(
    const void* buf, Allocator* alloc, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(buf));
  if (meta.isFixArray()) {
    return Array<std::string_view>(
        ptr + block.byteOffset,
        meta.count(),
        alloc,
        meta.dflt<std::string_view>(),
        OffsetBitMask(ptr + hasOffset_, meta.tag()));
  }
  int64_t offset = *reinterpret_cast<int64_t*>(ptr + block.byteOffset);
  if (offset == 0) {
    return Array<std::string_view>(nullptr, 0, alloc);
  }
  auto p = readBufferAndSize(alloc, offset);
  if (p.second == 0) {
    return Array<std::string_view>(nullptr, 0, alloc);
  }
  return Array<std::string_view>(
      p.first,
      p.second,
      alloc,
      meta.dflt<std::string_view>(),
      OffsetBitMask(ptr + hasOffset_, meta.tag()));
}

template <class T>
struct Accessor::MGetImpl {
  static Array<T> mget(
      const Accessor* accessor,
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->mgetNumeric<T>(buf, alloc, meta);
  }
};

template <>
struct Accessor::MGetImpl<bool> {
  static Array<bool> mget(
      const Accessor* accessor,
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->mgetBool(buf, alloc, meta);
  }
};

template <>
struct Accessor::MGetImpl<std::string_view> {
  static Array<std::string_view> mget(
      const Accessor* accessor,
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->mgetString(buf, alloc, meta);
  }
};

template <>
struct Accessor::MGetImpl<std::string> {
  static Array<std::string> mget(
      const Accessor* accessor,
      const void* buf,
      Allocator* alloc,
      const FieldMeta& meta) {
    return accessor->mgetString(buf, alloc, meta).toStdStringArray();
  }
};

inline size_t Accessor::bufferSize() const {
  return bufferSize_;
}

inline size_t Accessor::bitOffset() const {
  return bitOffset_;
}

inline const FieldBlock& Accessor::getFieldBlock(const FieldMeta& meta) const {
  return blocks_[meta.tag()];
}

template <class T>
bool Accessor::mergeArrayImpl(
    void* buf, Allocator* alloc,
    const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
    const FieldMeta& meta) const {
  Array<T> src = other->mget<T>(srcBuf, srcAlloc, meta);
  size_t size = src.size();
  if (meta.isVarArray()) {
    if (!resetArray(buf, alloc, meta)) {
      CRYSTAL_LOG(ERROR) << "reset array '" << meta.name() << "' failed";
      return false;
    }
    if (!buildVarArray(buf, alloc, meta, size)) {
      CRYSTAL_LOG(ERROR) << "build array '" << meta.name() << "' failed";
      return false;
    }
  }
  Array<T> dst = mget<T>(buf, alloc, meta);
  for (size_t i = 0; i < size; ++i) {
    if (!dst.set(i, src.get(i))) {
      return false;
    }
  }
  return true;
}

}  // namespace crystal
