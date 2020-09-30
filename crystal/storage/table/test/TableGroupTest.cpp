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

#include "crystal/memory/SysAllocator.h"
#include "crystal/storage/builder/RecordBuilder.h"
#include "crystal/storage/table/TableGroup.h"
#include "crystal/storage/table/test/TableGroupTest.h"
#include "crystal/strategy/DefaultStrategy.h"
#include "crystal/strategy/Hash.h"

using namespace crystal;

TEST_F(TableGroupTest, TableGroup_write) {
  TableGroupConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  TableGroup tablegroup(config);
  EXPECT_TRUE(tablegroup.init(path.c_str(), false));

  Table* table = tablegroup.getTable("menu");

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_EQ(-1, table->find(i));
    EXPECT_TRUE(table->insert(i, i));
    EXPECT_EQ((i % table->getKVSegmentCount()) << 32 | i, table->find(i));
  }

  RecordBuilder<SysAllocator> builder(j["table"]["menu"], true);
  builder.init(nullptr);

  const char* cson = R"(
      {
        menuId=1000,
        status=2,
        keyword="menu",
        content="menu content"
      }
      )";

  Record record = builder.build();
  decode(parseCson(cson), record);
  record.set<uint64_t>("__id", 10);

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    KV* kv = table->getKVSegmentById(i);
    EXPECT_FALSE(kv->exist(i));
    EXPECT_TRUE(kv->add(i, record));
    EXPECT_TRUE(kv->exist(i));
    if (i > 100) {
      EXPECT_TRUE(kv->remove(i));
      EXPECT_FALSE(kv->exist(i));
    }
  }

  Index* index = table->getIndexSegmentByToken("status", hashToken(2));
  EXPECT_NE(index, nullptr);

  DefaultStrategy strategy(index->keyMeta());
  auto keys = strategy.getIndexKeys(record);
  EXPECT_TRUE(index->add(keys[0], record));

  AnyPostingList pl = index->getPostingList(hashToken(2));
  EXPECT_TRUE(get(pl)->exist(10));
  EXPECT_EQ(1, get(pl)->size());

  AnyPostingListIterator it = get(pl)->iterator();
  EXPECT_EQ(10, get(it)->value()->id);

  tablegroup.dump();
}

TEST_F(TableGroupTest, TableGroup_read) {
  TableGroupConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  TableGroup tablegroup(config);
  EXPECT_TRUE(tablegroup.init(path.c_str(), true));

  Table* table = tablegroup.getTable("menu");

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_EQ((i % table->getKVSegmentCount()) << 32 | i, table->find(i));
    KV* kv = table->getKVSegmentById(i);
    if (i > 100) {
      EXPECT_FALSE(kv->exist(i));
    } else {
      EXPECT_TRUE(kv->exist(i));
      Record record = kv->createRecord();
      EXPECT_TRUE(kv->get(i, record));
      EXPECT_EQ(2, record.get<uint32_t>("status"));
    }
  }

  Index* index = table->getIndexSegmentByToken("status", hashToken(2));

  AnyPostingList pl = index->getPostingList(hashToken(2));
  EXPECT_TRUE(get(pl)->exist(10));
  EXPECT_EQ(1, get(pl)->size());

  AnyPostingListIterator it = get(pl)->iterator();
  EXPECT_EQ(10, get(it)->value()->id);
}
