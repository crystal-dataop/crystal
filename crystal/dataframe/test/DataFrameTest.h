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
#include "crystal/storage/table/TableFactory.h"

namespace crystal {

class DataFrameTest : public ::testing::Test {
 protected:
  std::string path = getProcessName() + "_data";
  std::filesystem::path conf =
    std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()
    / "storage" / "table" / "test" / "tablegroup.cson";

  void SetUp() override {
    static bool sOnce = true;
    if (sOnce) {
      std::filesystem::remove_all(path);
      prepare();
      sOnce = false;
    }
  }

  void prepare() {
    TableFactory factory;
    factory.load(conf.c_str(), path.c_str(), false);

    auto builder = factory.getTableGroupBuilder("restaurant");
    {
      std::string cson;
      std::filesystem::path data =
        std::filesystem::path(__FILE__)
        .parent_path().parent_path().parent_path()
        / "storage" / "builder" / "test" / "menudata.cson";
      readFile(data.c_str(), cson);
      builder->add("menu._", parseCson(cson));
    }
    {
      std::string cson;
      std::filesystem::path data =
        std::filesystem::path(__FILE__)
        .parent_path().parent_path().parent_path()
        / "storage" / "builder" / "test" / "fooddata.cson";
      readFile(data.c_str(), cson);
      builder->add("food._", parseCson(cson));
    }
    factory.dump();
  }
};

} // namespace crystal
