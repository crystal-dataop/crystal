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

#include "crystal/record/DynamicEncoding.h"
#include "crystal/record/test/RecordTest.h"

using namespace crystal;

TEST_F(RecordTest, genRecord) {
  Record record = MakeRecord();
  {
    EXPECT_EQ(1, record.get<uint64_t>(0));
    EXPECT_EQ(100, record.get<uint64_t>(1));
    EXPECT_STREQ("menu", record.get<string>(2).str().c_str());
    auto& foods = record.get<vector<FoodRecord>>(3);
    EXPECT_EQ(1, foods.size());
    FoodRecord& food = foods[0];
    EXPECT_EQ(2, food.get<uint64_t>(0));
    EXPECT_STREQ("food", food.get<string>(1).str().c_str());
    EXPECT_FLOAT_EQ(20, food.get<float>(2));
    EXPECT_TRUE(food.get<bool>(3));
    EXPECT_EQ(10, record.get<uint64_t>(4));
  }
  {
    auto& t = record.toTuple();
    EXPECT_EQ(1, get<0>(t));
    EXPECT_EQ(100, get<1>(t));
    EXPECT_STREQ("menu", get<2>(t).str().c_str());
    auto& foods = get<3>(t);
    EXPECT_EQ(1, foods.size());
    auto& food = foods[0].toTuple();
    EXPECT_EQ(2, get<0>(food));
    EXPECT_STREQ("food", get<1>(food).str().c_str());
    EXPECT_FLOAT_EQ(20, get<2>(food));
    EXPECT_TRUE(get<3>(food));
    EXPECT_EQ(10, get<4>(t));
  }
}

TEST_F(RecordTest, encoding) {
  Record record = MakeRecord();
  dynamic j = encode(record);
  EXPECT_STREQ(R"([1,100,"menu",[[2,"food",20,true]],10])", toJson(j).c_str());

  dynamic m = encode(record.untypedTuple().meta_);
  EXPECT_STREQ(R"([{"count":1,"type":"UINT64"},{"count":1,"type":"UINT64"},{"count":1,"type":"STRING"},{"count":0,"type":[{"type":"UINT64","count":1},{"type":"STRING","count":1},{"type":"FLOAT","count":1},{"type":"BOOL","count":1}]},{"count":1,"type":"UINT64"}])", toJson(m).c_str());

  dynamic merged = mergeTupleValueAndMeta(j, m);
  EXPECT_STREQ(R"([{"count":1,"value":1,"type":"UINT64"},{"count":1,"value":100,"type":"UINT64"},{"count":1,"value":"menu","type":"STRING"},{"count":0,"value":[[{"count":1,"value":2,"type":"UINT64"},{"count":1,"value":"food","type":"STRING"},{"count":1,"value":20,"type":"FLOAT"},{"count":1,"value":true,"type":"BOOL"}]],"type":[{"type":"UINT64","count":1},{"type":"STRING","count":1},{"type":"FLOAT","count":1},{"type":"BOOL","count":1}]},{"count":1,"value":10,"type":"UINT64"}])", toJson(merged).c_str());

  Record dec;
  decode(merged, dec);
  {
    auto& t = dec.toTuple();
    EXPECT_EQ(1, get<0>(t));
    EXPECT_EQ(100, get<1>(t));
    EXPECT_STREQ("menu", get<2>(t).str().c_str());
    auto& foods = get<3>(t);
    EXPECT_EQ(1, foods.size());
    auto& food = foods[0].toTuple();
    EXPECT_EQ(2, get<0>(food));
    EXPECT_STREQ("food", get<1>(food).str().c_str());
    EXPECT_FLOAT_EQ(20, get<2>(food));
    EXPECT_TRUE(get<3>(food));
    EXPECT_EQ(10, get<4>(t));
  }
}
