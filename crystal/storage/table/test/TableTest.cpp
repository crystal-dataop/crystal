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

#include "crystal/foundation/SystemUtil.h"
#include "crystal/memory/SysAllocator.h"
#include "crystal/storage/builder/RecordBuilder.h"
#include "crystal/storage/table/Table.h"
#include "crystal/strategy/DefaultStrategy.h"
#include "crystal/strategy/Hash.h"

using namespace crystal;

class TableTest : public ::testing::Test {
 protected:
  std::string path = getProcessName() + "_data";
  const char* conf = R"(
      {
        record=[
          { tag=1, name="menuId", type="uint64" },
          { tag=2, name="status", type="int32", bits=4, default=1 },
          { tag=3, name="keyword", type="string" },
          { tag=4, name="content", type="string" },
          { tag=5, name="food", type="string", count=0 }
        ],
        key="menuId",
        value=["menuId", "status", "content", "food"],
        bucket=10000,
        index=[
          {
            type="bitmap",
            key="status"
          }
        ]
      }
      )";
  const char* cson = R"(
      {
        menuId=1000,
        status=2,
        keyword="menu",
        content="menu content"
      }
      )";

  void SetUp() override {
    static bool sOnce = true;
    if (sOnce) {
      std::filesystem::remove_all(path);
      sOnce = false;
    }
  }
};

TEST_F(TableTest, write) {
  TableConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  Table table(config);
  EXPECT_TRUE(table.init(path.c_str(), false));

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_EQ(-1, table.find(i));
    EXPECT_TRUE(table.insert(i, i));
    EXPECT_EQ((i % table.getKVSegmentCount()) << 32 | i, table.find(i));
  }

  RecordBuilder<SysAllocator> builder(parseCson(conf), true);
  builder.init(nullptr);

  Record record = builder.build();
  decode(parseCson(cson), record);
  record.set<uint64_t>("__id", 10);

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    KV* kv = table.getKVById(i);
    EXPECT_FALSE(kv->exist(i));
    EXPECT_TRUE(kv->add(i, record));
    EXPECT_TRUE(kv->exist(i));
    if (i > 100) {
      EXPECT_TRUE(kv->remove(i));
      EXPECT_FALSE(kv->exist(i));
    }
  }

  Index* index = table.getIndexByToken("status", hashToken(2));
  EXPECT_NE(index, nullptr);

  DefaultStrategy strategy(index->keyMeta());
  auto keys = strategy.getIndexKeys(record);
  EXPECT_TRUE(index->add(keys[0], record));

  AnyPostingList pl = index->getPostingList(hashToken(2));
  EXPECT_TRUE(get(pl)->exist(10));
  EXPECT_EQ(1, get(pl)->size());

  AnyPostingListIterator it = get(pl)->iterator();
  EXPECT_EQ(10, get(it)->value()->id);

  table.dump();
}

TEST_F(TableTest, read) {
  TableConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  Table table(config);
  EXPECT_TRUE(table.init(path.c_str(), true));

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_EQ((i % table.getKVSegmentCount()) << 32 | i, table.find(i));
    KV* kv = table.getKVById(i);
    if (i > 100) {
      EXPECT_FALSE(kv->exist(i));
    } else {
      EXPECT_TRUE(kv->exist(i));
      Record record = kv->createRecord();
      EXPECT_TRUE(kv->get(i, record));
      EXPECT_EQ(2, record.get<uint32_t>("status"));
    }
  }

  Index* index = table.getIndexByToken("status", hashToken(2));

  AnyPostingList pl = index->getPostingList(hashToken(2));
  EXPECT_TRUE(get(pl)->exist(10));
  EXPECT_EQ(1, get(pl)->size());

  AnyPostingListIterator it = get(pl)->iterator();
  EXPECT_EQ(10, get(it)->value()->id);
}
