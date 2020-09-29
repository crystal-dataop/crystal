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

#include <gtest/gtest.h>

#include "crystal/serializer/record/FieldBlock.h"

using namespace crystal;

TEST(FieldBlock, single) {
  FieldMeta meta("field", 1, DataType::INT8, 8, 1);
  FieldBlock block(meta.tag());

  block.calculateSize(meta);
  block.generateMask(meta);

  EXPECT_STREQ(
      R"({bitOffset=0,bitSize=0,byteOffset=0,byteSize=1,itemBitSize=0,itemByteSize=1,mask="0:0:0",tag=1})",
      block.toString().c_str());
}

TEST(FieldBlock, bool) {
  FieldMeta meta("field", 1, DataType::BOOL, 1, 1);
  FieldBlock block(meta.tag());

  block.calculateSize(meta);
  block.generateMask(meta);

  EXPECT_STREQ(
      R"({bitOffset=0,bitSize=1,byteOffset=0,byteSize=0,itemBitSize=1,itemByteSize=0,mask="63:8000000000000000:0",tag=1})",
      block.toString().c_str());
}

TEST(FieldBlock, string) {
  FieldMeta meta("field", 1, DataType::STRING, 0, 1, "100");
  FieldBlock block(meta.tag());

  block.calculateSize(meta);
  block.generateMask(meta);

  EXPECT_STREQ(
      R"({bitOffset=0,bitSize=0,byteOffset=0,byteSize=8,itemBitSize=0,itemByteSize=8,mask="0:0:0",tag=1})",
      block.toString().c_str());
}

TEST(FieldBlock, fixarray) {
  FieldMeta meta("field", 1, DataType::STRING, 0, 10, "100");
  FieldBlock block(meta.tag());

  block.calculateSize(meta);
  block.generateMask(meta);

  EXPECT_STREQ(
      R"({bitOffset=0,bitSize=0,byteOffset=0,byteSize=80,itemBitSize=0,itemByteSize=8,mask="0:0:0",tag=1})",
      block.toString().c_str());
}

TEST(FieldBlock, vararray) {
  FieldMeta meta("field", 1, DataType::STRING, 0, 0, "100");
  FieldBlock block(meta.tag());

  block.calculateSize(meta);
  block.generateMask(meta);

  EXPECT_STREQ(
      R"({bitOffset=0,bitSize=0,byteOffset=0,byteSize=8,itemBitSize=0,itemByteSize=8,mask="0:0:0",tag=1})",
      block.toString().c_str());
}
