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

#include "crystal/foundation/json.h"
#include "crystal/serializer/record/FieldMeta.h"
#include "crystal/serializer/record/detail/Bitset.h"

namespace crystal {

struct FieldBlock {
  int tag{0};

  size_t byteOffset{0};
  size_t byteSize{0};
  size_t itemByteSize{0};

  size_t bitOffset{0};
  size_t bitSize{0};
  size_t itemBitSize{0};

  Bitset::Mask mask;

  explicit FieldBlock(int tag_ = 0) : tag(tag_) {}

  void calculateSize(const FieldMeta& meta);

  size_t alignBytes(size_t pos);
  size_t alignBits(size_t pos);

  void generateMask(const FieldMeta& meta);

  size_t getOffset(const FieldMeta& meta) const;
  size_t getItemSize(const FieldMeta& meta) const;

  dynamic toDynamic() const;
  std::string toString() const;
};

}  // namespace crystal
