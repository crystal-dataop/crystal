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

#include "crystal/memory/SysAllocator.h"
#include "crystal/serializer/record/Record.h"

using namespace crystal;

TEST(Record, all) {
  RecordMeta meta;

  meta.addMeta(FieldMeta("field1", 1, DataType::BOOL, 1, 1));
  meta.addMeta(FieldMeta("field2", 2, DataType::INT8, 8, 1, "50"));
  meta.addMeta(FieldMeta("field3", 3, DataType::INT8, 4, 1));
  meta.addMeta(FieldMeta("field4", 4, DataType::FLOAT, 32, 1));
  meta.addMeta(FieldMeta("field5", 5, DataType::STRING, 0, 1));
  meta.addMeta(FieldMeta("field6", 6, DataType::BOOL, 1, 10));
  meta.addMeta(FieldMeta("field7", 7, DataType::INT8, 8, 10, "50"));
  meta.addMeta(FieldMeta("field8", 8, DataType::INT8, 4, 10));
  meta.addMeta(FieldMeta("field9", 9, DataType::FLOAT, 32, 10));
  meta.addMeta(FieldMeta("field10", 10, DataType::STRING, 0, 10));
  meta.addMeta(FieldMeta("field11", 11, DataType::BOOL, 1, 0));
  meta.addMeta(FieldMeta("field12", 12, DataType::INT8, 8, 0, "50"));
  meta.addMeta(FieldMeta("field13", 13, DataType::INT8, 4, 0));
  meta.addMeta(FieldMeta("field14", 14, DataType::FLOAT, 32, 0));
  meta.addMeta(FieldMeta("field15", 15, DataType::STRING, 0, 0));

  Accessor accessor(meta);

  SysAllocator allocator;
  void* buf = allocator.address(allocator.allocate(accessor.bufferSize()));
  memset(buf, 0, accessor.bufferSize());

  Record record;
  record.init(&meta, &accessor, &allocator, buf);
  record.reset();

  EXPECT_EQ(false, record.get<bool>(*meta.getMeta(1)));
  EXPECT_EQ(50, record.get<int8_t>(*meta.getMeta(2)));
  EXPECT_EQ(0, record.get<int8_t>(*meta.getMeta(3)));
  EXPECT_EQ(0, record.get<float>(*meta.getMeta(4)));
  EXPECT_EQ("", record.get<std::string_view>(*meta.getMeta(5)));

  EXPECT_TRUE(record.set<bool>(*meta.getMeta(1), true));
  EXPECT_TRUE(record.set<int8_t>(*meta.getMeta(2), 100));
  EXPECT_TRUE(record.set<int8_t>(*meta.getMeta(3), 10));
  EXPECT_TRUE(record.set<float>(*meta.getMeta(4), 1.23));
  EXPECT_TRUE(record.set<std::string_view>(*meta.getMeta(5), "string"));

  EXPECT_EQ(true, record.get<bool>(*meta.getMeta(1)));
  EXPECT_EQ(100, record.get<int8_t>(*meta.getMeta(2)));
  EXPECT_EQ(10, record.get<int8_t>(*meta.getMeta(3)));
  EXPECT_FLOAT_EQ(1.23, record.get<float>(*meta.getMeta(4)));
  EXPECT_EQ("string", record.get<std::string_view>(*meta.getMeta(5)));

  Array<bool> a6 = record.get<Array<bool>>(*meta.getMeta(6));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(false, a6.get(i));
    EXPECT_TRUE(a6.set(i, true));
    EXPECT_EQ(true, a6.get(i));
  }
  Array<int8_t> a7 = record.get<Array<int8_t>>(*meta.getMeta(7));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(50, a7.get(i));
    EXPECT_TRUE(a7.set(i, i));
    EXPECT_EQ(i, a7.get(i));
  }
  Array<int8_t> a8 = record.get<Array<int8_t>>(*meta.getMeta(8));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(0, a8.get(i));
    EXPECT_TRUE(a8.set(i, i));
    EXPECT_EQ(i, a8.get(i));
  }
  Array<float> a9 = record.get<Array<float>>(*meta.getMeta(9));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_FLOAT_EQ(0, a9.get(i));
    EXPECT_TRUE(a9.set(i, i * 0.1));
    EXPECT_FLOAT_EQ(i * 0.1, a9.get(i));
  }
  Array<std::string_view> a10 =
    record.get<Array<std::string_view>>(*meta.getMeta(10));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ("", a10.get(i));
    EXPECT_TRUE(a10.set(i, "string"));
    EXPECT_EQ("string", a10.get(i));
  }

  EXPECT_TRUE(record.rebuildVarArray(*meta.getMeta(11), 10));
  Array<bool> a11 = record.get<Array<bool>>(*meta.getMeta(11));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(false, a11.get(i));
    EXPECT_TRUE(a11.set(i, true));
    EXPECT_EQ(true, a11.get(i));
  }
  EXPECT_TRUE(record.rebuildVarArray(*meta.getMeta(12), 10));
  Array<int8_t> a12 = record.get<Array<int8_t>>(*meta.getMeta(12));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(50, a12.get(i));
    EXPECT_TRUE(a12.set(i, i));
    EXPECT_EQ(i, a12.get(i));
  }
  EXPECT_TRUE(record.rebuildVarArray(*meta.getMeta(13), 10));
  Array<int8_t> a13 = record.get<Array<int8_t>>(*meta.getMeta(13));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(0, a13.get(i));
    EXPECT_TRUE(a13.set(i, i));
    EXPECT_EQ(i, a13.get(i));
  }
  EXPECT_TRUE(record.rebuildVarArray(*meta.getMeta(14), 10));
  Array<float> a14 = record.get<Array<float>>(*meta.getMeta(14));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_FLOAT_EQ(0, a14.get(i));
    EXPECT_TRUE(a14.set(i, i * 0.1));
    EXPECT_FLOAT_EQ(i * 0.1, a14.get(i));
  }
  EXPECT_TRUE(record.rebuildVarArray(*meta.getMeta(15), 10));
  Array<std::string_view> a15 =
      record.get<Array<std::string_view>>(*meta.getMeta(15));
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ("", a15.get(i));
    EXPECT_TRUE(a15.set(i, "string"));
    EXPECT_EQ("string", a15.get(i));
  }

  json::serialization_opts opts;
  opts.conf_style = true;
  opts.double_mode = double_conversion::DoubleToStringConverter::FIXED;
  opts.double_num_digits = 2;
  opts.sort_keys = true;
  EXPECT_STREQ(
      R"({field1=true)"
      R"(,field10=["string","string","string","string","string","string","string","string","string","string"])"
      R"(,field11=[true,true,true,true,true,true,true,true,true,true])"
      R"(,field12=[0,1,2,3,4,5,6,7,8,9])"
      R"(,field13=[0,1,2,3,4,5,6,7,8,9])"
      R"(,field14=[0.00,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.80,0.90])"
      R"(,field15=["string","string","string","string","string","string","string","string","string","string"])"
      R"(,field2=100)"
      R"(,field3=10)"
      R"(,field4=1.23)"
      R"(,field5="string")"
      R"(,field6=[true,true,true,true,true,true,true,true,true,true])"
      R"(,field7=[0,1,2,3,4,5,6,7,8,9])"
      R"(,field8=[0,1,2,3,4,5,6,7,8,9])"
      R"(,field9=[0.00,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.80,0.90]})",
      record.toString(opts).c_str());
}
