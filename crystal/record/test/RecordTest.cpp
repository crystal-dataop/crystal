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

#include "crystal/record/RecordConfig.h"
#include "crystal/record/test/record.gen.h"
#include "crystal/record/test/RecordTest.h"

using namespace crystal;

TEST_F(RecordTest, genRecord) {
  dynamic j = parseCson(conf);
  RecordConfig config = parseRecordConfig(j);
  untyped_tuple::meta meta = config.buildRecordMeta("*", true);

  Record record(meta);
  {
    record.__id() = 1;
    record.menuId() = 100;
    record.name() = "menu";
    FoodRecord food(untyped_tuple::meta{meta[3].submeta});
    food.foodId() = 2;
    food.name() = "food";
    food.price() = 20;
    food.onsale() = true;
    auto& foods = record.food();
    foods.assign(1, food);
    record.restaurantId() = 10;
  }
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
