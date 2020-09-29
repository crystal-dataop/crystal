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

#include "crystal/serializer/record/Accessor.h"

#include "crystal/math/Div.h"

namespace crystal {

Accessor::Accessor(const RecordMeta& recordMeta) {
  int blockCount = recordMeta.maxTag() + 1;
  blocks_.resize(blockCount);
  for (auto& meta : recordMeta) {
    FieldBlock block(meta.tag());
    block.calculateSize(meta);
    blocks_[block.tag] = block;
  }

  size_t byteSize = 0;
  for (auto& meta : recordMeta) {
    FieldBlock& block = blocks_[meta.tag()];
    if (!meta.isCompact() || meta.isVarArray()) {
      byteSize = block.alignBytes(byteSize);
    }
  }

  size_t bitSize = 0;
  for (auto& meta : recordMeta) {
    FieldBlock& block = blocks_[meta.tag()];
    if (meta.isCompact() && !meta.isVarArray()) {
      bitSize = block.alignBits(bitSize);
    }
    if (meta.isCompact() && !meta.isArray()) {
      block.generateMask(meta);
    }
  }

  bitOffset_ = byteSize;
  byteSize += div64(bitSize) * 8;   // Bitset required multiples of uint64
  hasOffset_ = byteSize;
  byteSize += div8(blockCount);
  bufferSize_ = div8(byteSize) * 8;
}

//////////////////////////////////////////////////////////////////////

bool Accessor::hasField(const void* buf, const FieldMeta& meta) const {
  uint8_t* ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(buf));
  return OffsetBitMask{ptr + hasOffset_, meta.tag()}.isSet();
}

void Accessor::setHasField(void* buf, const FieldMeta& meta, bool set) const {
  uint8_t* ptr = reinterpret_cast<uint8_t*>(buf);
  OffsetBitMask{ptr + hasOffset_, meta.tag()}.set(set);
}

static bool resetOneImpl(
    const FieldMeta& meta,
    uint8_t* buf,
    size_t itemOffset,
    Allocator* alloc) {
  switch (meta.type()) {
    case DataType::BOOL:
      if (!Bitset(1, meta.dflt<bool>()).serialize(buf, itemOffset)) {
        return false;
      }
      break;

#define RESET(type, enum_type)                                        \
  case DataType::enum_type:                                           \
    if (meta.isCompact()) {                                           \
      if (!Bitset(meta.bits(), meta.dflt<type>())                     \
              .serialize(buf, itemOffset)) {                          \
        return false;                                                 \
      }                                                               \
    } else {                                                          \
      *reinterpret_cast<type*>(buf + itemOffset) = meta.dflt<type>(); \
    }                                                                 \
    break;

    RESET(int8_t, INT8)
    RESET(int16_t, INT16)
    RESET(int32_t, INT32)
    RESET(int64_t, INT64)
    RESET(uint8_t, UINT8)
    RESET(uint16_t, UINT16)
    RESET(uint32_t, UINT32)
    RESET(uint64_t, UINT64)
    RESET(float, FLOAT)
    RESET(double, DOUBLE)

#undef RESET

    case DataType::STRING: {
      int64_t& offset = *reinterpret_cast<int64_t*>(buf + itemOffset);
      if (offset != 0) {
        alloc->deallocate(offset);
        offset = 0;
      }
      break;
    }
    default:
      CRYSTAL_LOG(ERROR)
          << "invalid data type: " << dataTypeToString(meta.type());
      return false;
  }
  return true;
}

bool Accessor::resetOne(
    void* buf, Allocator* alloc, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  size_t offset = block.getOffset(meta);
  uint8_t* ptr = meta.isCompact()
    ? reinterpret_cast<uint8_t*>(buf) + bitOffset_
    : reinterpret_cast<uint8_t*>(buf);
  if (!resetOneImpl(meta, ptr, offset, alloc)) {
    return false;
  }
  setHasField(buf, meta, false);
  return true;
}

bool Accessor::resetArray(
    void* buf, Allocator* alloc, const FieldMeta& meta) const {
  const FieldBlock& block = getFieldBlock(meta);
  size_t itemSize = block.getItemSize(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(buf);
  if (meta.isVarArray()) {
    int64_t& offset = *reinterpret_cast<int64_t*>(ptr + block.byteOffset);
    if (offset != 0) {
      auto p = readBufferAndSize(alloc, offset);
      uint8_t* pp = reinterpret_cast<uint8_t*>(p.first);
      for (uint32_t i = 0; i < p.second; ++i) {
        if (!resetOneImpl(meta, pp, itemSize * i, alloc)) {
          return false;
        }
      }
      alloc->deallocate(offset);
      offset = 0;
    }
  } else {
    size_t offset = block.getOffset(meta);
    uint8_t* pp = meta.isCompact()
      ? reinterpret_cast<uint8_t*>(buf) + bitOffset_
      : reinterpret_cast<uint8_t*>(buf);
    for (size_t i = 0; i < meta.count(); ++i) {
      if (!resetOneImpl(meta, pp, offset + itemSize * i, alloc)) {
        return false;
      }
    }
  }
  setHasField(buf, meta, false);
  return true;
}

bool Accessor::reset(
    void* buf, Allocator* alloc, const RecordMeta& recordMeta) const {
  for (auto& meta : recordMeta) {
    if (!(meta.isArray() ? resetArray(buf, alloc, meta)
                         : resetOne(buf, alloc, meta))) {
      return false;
    }
  }
  return true;
}

bool Accessor::mergeOne(
    void* buf, Allocator* alloc,
    const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
    const FieldMeta& meta) const {
  switch (meta.type()) {
#define COPY(type, enum_type)           \
  case DataType::enum_type:             \
    return set<type>(buf, alloc, meta,  \
                     other->get<type>(srcBuf, srcAlloc, meta));

    COPY(bool, BOOL)
    COPY(int8_t, INT8)
    COPY(int16_t, INT16)
    COPY(int32_t, INT32)
    COPY(int64_t, INT64)
    COPY(uint8_t, UINT8)
    COPY(uint16_t, UINT16)
    COPY(uint32_t, UINT32)
    COPY(uint64_t, UINT64)
    COPY(float, FLOAT)
    COPY(double, DOUBLE)
    COPY(std::string_view, STRING)

#undef COPY

    default:
      CRYSTAL_LOG(ERROR)
          << "invalid data type: " << dataTypeToString(meta.type());
      return false;
  }
  return true;
}

bool Accessor::mergeArray(
    void* buf, Allocator* alloc,
    const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
    const FieldMeta& meta) const {
  switch (meta.type()) {
#define COPY(type, enum_type)           \
  case DataType::enum_type:             \
    return mergeArrayImpl<type>(buf, alloc, srcBuf, srcAlloc, other, meta);

    COPY(bool, BOOL)
    COPY(int8_t, INT8)
    COPY(int16_t, INT16)
    COPY(int32_t, INT32)
    COPY(int64_t, INT64)
    COPY(uint8_t, UINT8)
    COPY(uint16_t, UINT16)
    COPY(uint32_t, UINT32)
    COPY(uint64_t, UINT64)
    COPY(float, FLOAT)
    COPY(double, DOUBLE)
    COPY(std::string_view, STRING)

#undef COPY

    default:
      CRYSTAL_LOG(ERROR)
          << "invalid data type: " << dataTypeToString(meta.type());
      return false;
  }
  return true;
}

bool Accessor::merge(
    void* buf, Allocator* alloc,
    const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
    const RecordMeta& recordMeta) const {
  for (auto& meta : recordMeta) {
    if (other->hasField(srcBuf, meta)) {
      if (!(meta.isArray()
            ? mergeArray(buf, alloc, srcBuf, srcAlloc, other, meta)
            : mergeOne(buf, alloc, srcBuf, srcAlloc, other, meta))) {
        return false;
      }
    }
  }
  return true;
}

bool Accessor::copy(
    void* buf, Allocator* alloc,
    const void* srcBuf, Allocator* srcAlloc, const Accessor* other,
    const RecordMeta& recordMeta) const {
  if (!reset(buf, alloc, recordMeta) ||
      !merge(buf, alloc, srcBuf, srcAlloc, other, recordMeta)) {
    return false;
  }
  return true;
}

bool Accessor::buildVarArray(
    void* buf, Allocator* alloc, const FieldMeta& meta, size_t size) const {
  if (!meta.isVarArray()) {
    return false;
  }
  const FieldBlock& block = getFieldBlock(meta);
  size_t itemSize = block.getItemSize(meta);
  uint8_t* ptr = reinterpret_cast<uint8_t*>(buf);
  int64_t& offset = *reinterpret_cast<int64_t*>(ptr + block.byteOffset);
  size_t chunkSize = meta.isCompact()
    ? div64(size * itemSize) * 8
    : (size * itemSize);
  offset = allocBuffer(alloc, chunkSize, true);
  if (offset == 0) {
    return false;
  }
  *address<uint32_t>(alloc, offset) = size;
  auto p = readBufferAndSize(alloc, offset);
  uint8_t* pp = reinterpret_cast<uint8_t*>(p.first);
  for (size_t i = 0; i < p.second; ++i) {
    if (!resetOneImpl(meta, pp, itemSize * i, alloc)) {
      return false;
    }
  }
  return true;
}

dynamic Accessor::toDynamic() const {
  dynamic blocks = dynamic::array;
  for (auto& block : blocks_) {
    if (block.tag != 0) {
      blocks.push_back(block.toDynamic());
    }
  }
  dynamic j = dynamic::object
    ("bitOffset", bitOffset_)
    ("bufferSize", bufferSize_)
    ("blocks", blocks);
  return j;
}

std::string Accessor::toString() const {
  return toCson(toDynamic());
}

}  // namespace crystal
