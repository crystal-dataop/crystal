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

#include <filesystem>
#include <gtest/gtest.h>

#include "crystal/foundation/File.h"
#include "crystal/storage/table/TableGroupConfig.h"

using namespace crystal;

TEST(TableGroupConfig, all) {
  std::filesystem::path file =
    std::filesystem::path(__FILE__).parent_path() / "tablegroup.cson";
  std::string conf;
  readFile(file.c_str(), conf);

  TableGroupConfig config;
  config.parse(parseCson(conf));

  EXPECT_STREQ("1.0", config.version().c_str());
  EXPECT_EQ(2, config.tableConfigs().size());
  EXPECT_EQ(DataType::UINT64,
            config.getRelatedFieldType(
                config
                .tableConfigs().at("food")
                .recordConfig().at("menuId")));
}
