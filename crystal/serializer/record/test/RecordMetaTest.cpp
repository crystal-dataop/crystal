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

#include "crystal/serializer/record/RecordMeta.h"

using namespace crystal;

TEST(RecordMeta, all) {
  RecordMeta meta;

  meta.addMeta(FieldMeta("field1", 1, DataType::INT8, 8, 1));
  meta.addMeta(FieldMeta("field2", 2, DataType::INT8, 8, 1, "100"));
  meta.addMeta(FieldMeta("field3", 3, DataType::STRING, 0, 1, "100"));
  meta.addMeta(FieldMeta("field4", 4, DataType::STRING, 0, 10, "100"));

  EXPECT_FALSE(meta.getMeta("field1")->isArray());
  EXPECT_EQ(100, meta.getMeta("field2")->dflt<int8_t>());
  EXPECT_STREQ("100", meta.getMeta("field3")->dflt<std::string>().c_str());
  EXPECT_TRUE(meta.getMeta("field4")->isArray());
  EXPECT_TRUE(meta.getMeta("field4")->isFixArray());

  EXPECT_STREQ(
      R"([{bits=8,compact=false,count=1,default=0,name="field1",tag=1,type="INT8"})"
      R"(,{bits=8,compact=false,count=1,default=100,name="field2",tag=2,type="INT8"})"
      R"(,{bits=0,compact=false,count=1,default="100",name="field3",tag=3,type="STRING"})"
      R"(,{bits=0,compact=false,count=10,default="100",name="field4",tag=4,type="STRING"}])",
      meta.toString().c_str());

  for (auto& fieldMeta : meta) {
    EXPECT_NE(DataType::UNKNOWN, fieldMeta.type());
  }

  meta.clear();

  EXPECT_STREQ("[]", meta.toString().c_str());
}
