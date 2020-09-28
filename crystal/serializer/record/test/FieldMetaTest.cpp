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

#include "crystal/serializer/record/FieldMeta.h"

using namespace crystal;

TEST(FieldMeta, single) {
  FieldMeta meta("field", 1, DataType::INT8, 8, 1);

  EXPECT_FALSE(meta.isArray());
  EXPECT_STREQ(
      R"({bits=8,compact=false,count=1,default=0,name="field",tag=1,type="INT8"})",
      meta.toString().c_str());
}

TEST(FieldMeta, compact) {
  FieldMeta meta("field", 1, DataType::INT8, 4, 1);

  EXPECT_TRUE(meta.isCompact());
  EXPECT_STREQ(
      R"({bits=4,compact=true,count=1,default=0,name="field",tag=1,type="INT8"})",
      meta.toString().c_str());
}

TEST(FieldMeta, dflt) {
  FieldMeta meta("field", 1, DataType::INT8, 8, 1, "100");

  EXPECT_EQ(100, meta.dflt<int8_t>());
  EXPECT_THROW(meta.dflt<std::string>(), RuntimeError);
  EXPECT_STREQ(
      R"({bits=8,compact=false,count=1,default=100,name="field",tag=1,type="INT8"})",
      meta.toString().c_str());
}

TEST(FieldMeta, string) {
  FieldMeta meta("field", 1, DataType::STRING, 0, 1, "100");

  EXPECT_STREQ("100", meta.dflt<std::string>().c_str());
  EXPECT_STREQ(
      R"({bits=0,compact=false,count=1,default="100",name="field",tag=1,type="STRING"})",
      meta.toString().c_str());
}

TEST(FieldMeta, array) {
  FieldMeta meta("field", 1, DataType::STRING, 0, 10, "100");

  EXPECT_TRUE(meta.isArray());
  EXPECT_TRUE(meta.isFixArray());
  EXPECT_STREQ(
      R"({bits=0,compact=false,count=10,default="100",name="field",tag=1,type="STRING"})",
      meta.toString().c_str());
}
