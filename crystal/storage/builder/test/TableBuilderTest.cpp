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
#include "crystal/storage/builder/TableBuilder.h"
#include "crystal/strategy/Hash.h"

using namespace crystal;

TEST(TableBuilder, all) {
  std::string path = getProcessName() + "_data";
  std::filesystem::remove_all(path);

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

  TableConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  Table table(config);
  EXPECT_TRUE(table.init(path.c_str(), false));

  TableBuilder builder(&table);

  {
    const char* cson = R"(
        {
          menuId=1000,
          status=2,
          keyword="menu",
          content="menu content",
          food=["a","b","c"]
        }
        )";
    EXPECT_TRUE(builder.add(parseCson(cson)));

    EXPECT_EQ(1, table.find(hashToken<uint64_t>(1000)));

    KV* kv = table.getKVById(1);
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

    Index* index = table.getIndexByToken("status", hashToken(2));

    AnyPostingList pl = index->getPostingList(hashToken(2));
    EXPECT_TRUE(get(pl)->exist(1));
    EXPECT_EQ(1, get(pl)->size());
    AnyPostingListIterator it = get(pl)->iterator();
    EXPECT_EQ(1, get(it)->value()->id);
  }
  {
    const char* cson = R"(
        {
          menuId=1000,
          content="menu content update"
        }
        )";
    EXPECT_TRUE(builder.update(parseCson(cson)));

    EXPECT_EQ(1, table.find(hashToken<uint64_t>(1000)));

    KV* kv = table.getKVById(1);
    EXPECT_TRUE(kv->exist(1));

    Record record = kv->createRecord();
    EXPECT_TRUE(kv->get(1, record));
    EXPECT_EQ(1000, record.get<uint64_t>("menuId"));
    EXPECT_STREQ("menu content update",
                 record.get<std::string>("content").c_str());
  }
  {
    const char* cson = R"(
        {
          menuId=1000,
          status=2
        }
        )";
    EXPECT_TRUE(builder.remove(parseCson(cson)));

    EXPECT_EQ(-1, table.find(hashToken<uint64_t>(1000)));

    KV* kv = table.getKVById(1);
    EXPECT_FALSE(kv->exist(1));

    Index* index = table.getIndexByToken("status", hashToken(2));

    AnyPostingList pl = index->getPostingList(hashToken(2));
    EXPECT_FALSE(get(pl)->exist(1));
    EXPECT_EQ(0, get(pl)->size());
  }
}
