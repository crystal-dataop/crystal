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

#include "crystal/serializer/record/RecordConfig.h"

using namespace crystal;

TEST(FieldConfig, single) {
  FieldConfig config;
  config.parse(parseCson(R"({ tag=1, name="menuId", type="uint64" })"));

  EXPECT_EQ("menuId", config.name());
  EXPECT_EQ(1, config.tag());
  EXPECT_EQ(DataType::UINT64, config.type());
  EXPECT_EQ(64, config.bits());
  EXPECT_EQ(1, config.count());
  EXPECT_TRUE(config.dflt().isNull());
  EXPECT_FALSE(config.isRelated());

  EXPECT_STREQ(R"({bits=64,count=1,name="menuId",tag=1,type="UINT64"})",
               config.toString().c_str());
}

TEST(FieldConfig, array) {
  FieldConfig config;
  config.parse(parseCson(R"({ tag=2, name="food", type="string", count=0 })"));

  EXPECT_EQ("food", config.name());
  EXPECT_EQ(2, config.tag());
  EXPECT_EQ(DataType::STRING, config.type());
  EXPECT_EQ(0, config.bits());
  EXPECT_EQ(0, config.count());
  EXPECT_TRUE(config.dflt().isNull());
  EXPECT_FALSE(config.isRelated());

  EXPECT_STREQ(R"({bits=0,count=0,name="food",tag=2,type="STRING"})",
               config.toString().c_str());
}

TEST(FieldConfig, dflt) {
  FieldConfig config;
  config.parse(parseCson(R"({ tag=3, name="status", type="int32", bits=4, default=1 })"));

  EXPECT_EQ("status", config.name());
  EXPECT_EQ(3, config.tag());
  EXPECT_EQ(DataType::INT32, config.type());
  EXPECT_EQ(4, config.bits());
  EXPECT_EQ(1, config.count());
  EXPECT_EQ(1, config.dflt().getInt());
  EXPECT_FALSE(config.isRelated());

  EXPECT_STREQ(R"({bits=4,count=1,default=1,name="status",tag=3,type="INT32"})",
               config.toString().c_str());
}

TEST(FieldConfig, related) {
  FieldConfig config;
  config.parse(parseCson(R"({ tag=4, name="chefId", type="related", table="chef" })"));

  EXPECT_EQ("chefId", config.name());
  EXPECT_EQ(4, config.tag());
  EXPECT_EQ(DataType::UINT64, config.type());
  EXPECT_EQ(64, config.bits());
  EXPECT_EQ(1, config.count());
  EXPECT_TRUE(config.dflt().isNull());
  EXPECT_STREQ("chef", config.related().c_str());

  EXPECT_STREQ(
      R"({bits=64,count=1,name="chefId",table="chef",tag=4,type="related"})",
      config.toString().c_str());
}
