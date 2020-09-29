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

#include "crystal/serializer/record/FieldBlock.h"

#include "crystal/foundation/String.h"
#include "crystal/type/DataType.h"

namespace crystal {

void FieldBlock::calculateSize(const FieldMeta& meta) {
  byteSize = 0;
  itemByteSize = 0;
  bitSize = 0;
  itemBitSize = 0;

  if (meta.isCompact()) {
    itemBitSize = meta.bits();
    bitSize = itemBitSize * meta.count();
  } else {
    itemByteSize = sizeOf(meta.type());
    byteSize = itemByteSize * meta.count();
  }
  if (meta.isVarArray()) {
    byteSize = sizeof(int64_t);
  }
}

size_t FieldBlock::alignBytes(size_t pos) {
  byteOffset = pos;
  return pos + byteSize;
}

size_t FieldBlock::alignBits(size_t pos) {
  bitOffset = pos;
  return pos + bitSize;
}

void FieldBlock::generateMask(const FieldMeta& meta) {
  if (meta.isCompact()) {
    Bitset bits(meta.bits());
    bits.generateMask(bitOffset, mask);
  }
}

size_t FieldBlock::getOffset(const FieldMeta& meta) const {
  return meta.isCompact() ? bitOffset : byteOffset;
}

size_t FieldBlock::getItemSize(const FieldMeta& meta) const {
  return meta.isCompact() ? itemBitSize : itemByteSize;
}

dynamic FieldBlock::toDynamic() const {
  dynamic j = dynamic::object
    ("tag", tag)
    ("byteOffset", byteOffset)
    ("byteSize", byteSize)
    ("itemByteSize", itemByteSize)
    ("bitOffset", bitOffset)
    ("bitSize", bitSize)
    ("itemBitSize", itemBitSize)
    ("mask",
     stringPrintf("%lld:%llx:%llx", mask.shift, mask.mask1, mask.mask2));
  return j;
}

std::string FieldBlock::toString() const {
  return toCson(toDynamic());
}

}  // namespace crystal
