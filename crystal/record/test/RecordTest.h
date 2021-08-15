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

#pragma once

#include <filesystem>
#include <gtest/gtest.h>

#include "crystal/foundation/File.h"
#include "crystal/foundation/SystemUtil.h"
#include "crystal/record/RecordConfig.h"
#include "crystal/record/test/record.gen.h"

namespace crystal {

class RecordTest : public ::testing::Test {
 protected:
  RecordConfig config;

  void SetUp() override {
    std::filesystem::path file =
      std::filesystem::path(__FILE__).parent_path() / "record.cson";
    std::string conf;
    readFile(file.c_str(), conf);
    config = parseRecordConfig(parseCson(conf));
  }

  Record MakeRecord() {
    untyped_tuple::meta meta = config.buildRecordMeta("*", true);
    Record record(meta);
    record.__id() = 1;
    record.menuId() = 100;
    record.name() = "menu";
    FoodRecord food = record.makeFoodRecord();
    food.foodId() = 2;
    food.name() = "food";
    food.price() = 20;
    food.onsale() = true;
    auto& foods = record.food();
    foods.assign(1, food);
    record.restaurantId() = 10;
    return record;
  }
};

} // namespace crystal
