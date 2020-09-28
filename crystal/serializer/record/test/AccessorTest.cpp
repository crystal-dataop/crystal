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

#include <gtest/gtest.h>
#include <string>

#include "crystal/memory/SysAllocator.h"
#include "crystal/serializer/record/Accessor.h"

using namespace crystal;

TEST(Accessor, all) {
  RecordMeta meta;

  meta.addMeta(FieldMeta("field1", 1, DataType::BOOL, 1, 1));
  meta.addMeta(FieldMeta("field2", 2, DataType::INT8, 8, 1, "50"));
  meta.addMeta(FieldMeta("field3", 3, DataType::INT8, 4, 1));
  meta.addMeta(FieldMeta("field4", 4, DataType::FLOAT, 32, 1));
  meta.addMeta(FieldMeta("field5", 5, DataType::STRING, 0, 1, "default"));
  meta.addMeta(FieldMeta("field6", 6, DataType::BOOL, 1, 10));
  meta.addMeta(FieldMeta("field7", 7, DataType::INT8, 8, 10, "50"));
  meta.addMeta(FieldMeta("field8", 8, DataType::INT8, 4, 10));
  meta.addMeta(FieldMeta("field9", 9, DataType::FLOAT, 32, 10));
  meta.addMeta(FieldMeta("field10", 10, DataType::STRING, 0, 10, "default"));
  meta.addMeta(FieldMeta("field11", 11, DataType::BOOL, 1, 0));
  meta.addMeta(FieldMeta("field12", 12, DataType::INT8, 8, 0, "50"));
  meta.addMeta(FieldMeta("field13", 13, DataType::INT8, 4, 0));
  meta.addMeta(FieldMeta("field14", 14, DataType::FLOAT, 32, 0));
  meta.addMeta(FieldMeta("field15", 15, DataType::STRING, 0, 0, "default"));

  Accessor accessor(meta);

  EXPECT_EQ(183, accessor.bitOffset());
  EXPECT_EQ(200, accessor.bufferSize());

  SysAllocator allocator;
  void* buf = allocator.address(allocator.allocate(accessor.bufferSize()));
  memset(buf, 0, accessor.bufferSize());
  accessor.reset(buf, &allocator, meta);

  EXPECT_EQ(false, accessor.get<bool>(buf, &allocator, *meta.getMeta(1)));
  EXPECT_EQ(50, accessor.get<int8_t>(buf, &allocator, *meta.getMeta(2)));
  EXPECT_EQ(0, accessor.get<int8_t>(buf, &allocator, *meta.getMeta(3)));
  EXPECT_FLOAT_EQ(0, accessor.get<float>(buf, &allocator, *meta.getMeta(4)));
  EXPECT_STREQ("default", std::string(accessor.get<std::string_view>(
          buf, &allocator, *meta.getMeta(5))).c_str());

  EXPECT_TRUE(accessor.set<bool>(buf, &allocator, *meta.getMeta(1), true));
  EXPECT_TRUE(accessor.set<int8_t>(buf, &allocator, *meta.getMeta(2), 100));
  EXPECT_TRUE(accessor.set<int8_t>(buf, &allocator, *meta.getMeta(3), 10));
  EXPECT_TRUE(accessor.set<float>(buf, &allocator, *meta.getMeta(4), 1.23));
  EXPECT_TRUE(accessor.set<std::string_view>(buf, &allocator, *meta.getMeta(5),
                                             "string"));

  EXPECT_EQ(true, accessor.get<bool>(buf, &allocator, *meta.getMeta(1)));
  EXPECT_EQ(100, accessor.get<int8_t>(buf, &allocator, *meta.getMeta(2)));
  EXPECT_EQ(10, accessor.get<int8_t>(buf, &allocator, *meta.getMeta(3)));
  EXPECT_FLOAT_EQ(1.23, accessor.get<float>(buf, &allocator, *meta.getMeta(4)));
  EXPECT_STREQ("string", std::string(accessor.get<std::string_view>(
          buf, &allocator, *meta.getMeta(5))).c_str());

  Array<bool> a6 = accessor.mget<bool>(buf, &allocator, *meta.getMeta(6));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(false, a6.get(i));
    EXPECT_TRUE(a6.set(i, true));
    EXPECT_EQ(true, a6.get(i));
  }
  Array<int8_t> a7 = accessor.mget<int8_t>(buf, &allocator, *meta.getMeta(7));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(50, a7.get(i));
    EXPECT_TRUE(a7.set(i, i));
    EXPECT_EQ(i, a7.get(i));
  }
  Array<int8_t> a8 = accessor.mget<int8_t>(buf, &allocator, *meta.getMeta(8));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(0, a8.get(i));
    EXPECT_TRUE(a8.set(i, i));
    EXPECT_EQ(i, a8.get(i));
  }
  Array<float> a9 = accessor.mget<float>(buf, &allocator, *meta.getMeta(9));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_FLOAT_EQ(0, a9.get(i));
    EXPECT_TRUE(a9.set(i, i * 0.1));
    EXPECT_FLOAT_EQ(i * 0.1, a9.get(i));
  }
  Array<std::string_view> a10 =
      accessor.mget<std::string_view>(buf, &allocator, *meta.getMeta(10));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_STREQ("default", std::string(a10.get(i)).c_str());
    EXPECT_TRUE(a10.set(i, "string"));
    EXPECT_STREQ("string", std::string(a10.get(i)).c_str());
  }

  EXPECT_TRUE(accessor.buildVarArray(buf, &allocator, *meta.getMeta(11), 10));
  Array<bool> a11 = accessor.mget<bool>(buf, &allocator, *meta.getMeta(11));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(false, a11.get(i));
    EXPECT_TRUE(a11.set(i, true));
    EXPECT_EQ(true, a11.get(i));
  }
  EXPECT_TRUE(accessor.buildVarArray(buf, &allocator, *meta.getMeta(12), 10));
  Array<int8_t> a12 = accessor.mget<int8_t>(buf, &allocator, *meta.getMeta(12));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(50, a12.get(i));
    EXPECT_TRUE(a12.set(i, i));
    EXPECT_EQ(i, a12.get(i));
  }
  EXPECT_TRUE(accessor.buildVarArray(buf, &allocator, *meta.getMeta(13), 10));
  Array<int8_t> a13 = accessor.mget<int8_t>(buf, &allocator, *meta.getMeta(13));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(0, a13.get(i));
    EXPECT_TRUE(a13.set(i, i));
    EXPECT_EQ(i, a13.get(i));
  }
  EXPECT_TRUE(accessor.buildVarArray(buf, &allocator, *meta.getMeta(14), 10));
  Array<float> a14 = accessor.mget<float>(buf, &allocator, *meta.getMeta(14));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_FLOAT_EQ(0, a14.get(i));
    EXPECT_TRUE(a14.set(i, i * 0.1));
    EXPECT_FLOAT_EQ(i * 0.1, a14.get(i));
  }
  EXPECT_TRUE(accessor.buildVarArray(buf, &allocator, *meta.getMeta(15), 10));
  Array<std::string_view> a15 =
      accessor.mget<std::string_view>(buf, &allocator, *meta.getMeta(15));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_STREQ("default", std::string(a15.get(i)).c_str());
    EXPECT_TRUE(a15.set(i, "string"));
    EXPECT_STREQ("string", std::string(a15.get(i)).c_str());
  }

  EXPECT_STREQ(
      R"({bitOffset=183,blocks=)"
      R"([{bitOffset=0,bitSize=1,byteOffset=0,byteSize=0,itemBitSize=1,itemByteSize=0,mask="63:8000000000000000:0",tag=1})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=0,byteSize=1,itemBitSize=0,itemByteSize=1,mask="0:0:0",tag=2})"
      R"(,{bitOffset=1,bitSize=4,byteOffset=0,byteSize=0,itemBitSize=4,itemByteSize=0,mask="59:7800000000000000:0",tag=3})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=1,byteSize=4,itemBitSize=0,itemByteSize=4,mask="0:0:0",tag=4})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=5,byteSize=8,itemBitSize=0,itemByteSize=8,mask="0:0:0",tag=5})"
      R"(,{bitOffset=5,bitSize=10,byteOffset=0,byteSize=0,itemBitSize=1,itemByteSize=0,mask="0:0:0",tag=6})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=13,byteSize=10,itemBitSize=0,itemByteSize=1,mask="0:0:0",tag=7})"
      R"(,{bitOffset=15,bitSize=40,byteOffset=0,byteSize=0,itemBitSize=4,itemByteSize=0,mask="0:0:0",tag=8})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=23,byteSize=40,itemBitSize=0,itemByteSize=4,mask="0:0:0",tag=9})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=63,byteSize=80,itemBitSize=0,itemByteSize=8,mask="0:0:0",tag=10})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=143,byteSize=8,itemBitSize=1,itemByteSize=0,mask="0:0:0",tag=11})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=151,byteSize=8,itemBitSize=0,itemByteSize=1,mask="0:0:0",tag=12})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=159,byteSize=8,itemBitSize=4,itemByteSize=0,mask="0:0:0",tag=13})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=167,byteSize=8,itemBitSize=0,itemByteSize=4,mask="0:0:0",tag=14})"
      R"(,{bitOffset=0,bitSize=0,byteOffset=175,byteSize=8,itemBitSize=0,itemByteSize=8,mask="0:0:0",tag=15}])"
      R"(,bufferSize=200})",
      accessor.toString().c_str());
}
