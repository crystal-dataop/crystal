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
#include "crystal/foundation/SystemUtil.h"
#include "crystal/storage/builder/TableGroupBuilder.h"
#include "crystal/strategy/Hash.h"

using namespace crystal;

TEST(TableGroupBuilder, all) {
  std::string path = getProcessName() + "_data";
  std::filesystem::remove_all(path);

  std::string conf;
  std::filesystem::path file =
    std::filesystem::path(__FILE__).parent_path().parent_path().parent_path()
    / "table" / "test" / "tablegroup.cson";
  readFile(file.c_str(), conf);

  TableGroupConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  TableGroup tablegroup(config);
  EXPECT_TRUE(tablegroup.init(path.c_str(), false));

  TableGroupBuilder builder(&tablegroup);

  {
    std::string cson;
    std::filesystem::path data =
      std::filesystem::path(__FILE__).parent_path() / "menudata.cson";
    readFile(data.c_str(), cson);
    EXPECT_TRUE(builder.add("menu._", parseCson(cson)));

    uint64_t id = (1ul << 32) | 1;
    Table* table = tablegroup.getTable("menu");
    EXPECT_EQ(id, table->find(hashToken<uint64_t>(1000)));

    KV* kv = table->getKVById(id);
    EXPECT_TRUE(kv->exist(1));

    Record record = kv->createRecord();
    EXPECT_TRUE(kv->get(1, record));
    EXPECT_EQ(1000, record.get<uint64_t>("menuId"));
    EXPECT_EQ(2, record.get<uint32_t>("status"));
    EXPECT_STREQ("menu content", record.get<std::string>("content").c_str());
    Array<std::string_view> a = record.get<Array<std::string_view>>("food");
    EXPECT_EQ(3, a.size());
    EXPECT_STREQ("a", std::string(a.get(0)).c_str());
    EXPECT_STREQ("b", std::string(a.get(1)).c_str());
    EXPECT_STREQ("c", std::string(a.get(2)).c_str());
  }
  {
    std::string cson;
    std::filesystem::path data =
      std::filesystem::path(__FILE__).parent_path() / "fooddata.cson";
    readFile(data.c_str(), cson);
    EXPECT_TRUE(builder.add("food._", parseCson(cson)));

    Table* table = tablegroup.getTable("food");
    EXPECT_EQ(1, table->find(hashToken<uint64_t>(100)));

    KV* kv = table->getKVById(1);
    EXPECT_TRUE(kv->exist(1));

    Record record = kv->createRecord();
    EXPECT_TRUE(kv->get(1, record));
    EXPECT_EQ(100, record.get<uint64_t>("foodId"));
    EXPECT_EQ(2, record.get<uint32_t>("status"));
    EXPECT_STREQ("food name", record.get<std::string>("name").c_str());
    EXPECT_STREQ("food desc", record.get<std::string>("desc").c_str());
    EXPECT_FLOAT_EQ(5.5, record.get<float>("price"));
    uint64_t id = (1ul << 32) | 1;
    EXPECT_EQ(id, record.get<uint64_t>("menuId"));
  }
}
