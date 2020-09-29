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

#include "crystal/memory/test/MemoryManagerTest.h"
#include "crystal/storage/builder/RecordBuilder.h"
#include "crystal/storage/index/bitmap/BitmapIndex.h"

using namespace crystal;

class BitmapIndexTest : public MemoryManagerTest {
 protected:
  const char* conf = R"(
      {
        record=[
          { tag=1, name="menuId", type="uint64" },
          { tag=2, name="status", type="int32", bits=4, default=1 },
          { tag=3, name="keyword", type="string" },
          { tag=4, name="content", type="string" },
          { tag=5, name="food", type="string", count=0 }
        ],
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
        menuId=1000
      }
      )";
};

TEST_F(BitmapIndexTest, write) {
  IndexConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j["index"][0], parseRecordConfig(j)));

  MemoryManager manager(path.c_str(), false);

  BitmapIndex index(&config);
  EXPECT_TRUE(index.init(&manager));

  index.createPostingList(100);
  AnyPostingList pl = index.getPostingList(100);
  EXPECT_TRUE(std::holds_alternative<BitmapPostingList>(pl));
  EXPECT_NE(nullptr, get(pl)->index());

  RecordBuilder<SysAllocator> builder(parseCson(conf), true);
  builder.init(nullptr);

  Record record = builder.build();
  decode(parseCson(cson), record);
  record.set<uint64_t>("__id", 10);

  EXPECT_FALSE(get(pl)->exist(10));

  BitmapPosting posting;
  EXPECT_TRUE(posting.parseFrom(record));
  EXPECT_EQ(10, posting.id);

  EXPECT_EQ(0, get(pl)->add(posting));
  EXPECT_TRUE(get(pl)->exist(10));
  EXPECT_EQ(1, get(pl)->size());

  AnyPostingListIterator it = get(pl)->iterator();
  EXPECT_EQ(10, get(it)->value()->id);

  manager.dump();
}

TEST_F(BitmapIndexTest, read) {
  IndexConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j["index"][0], parseRecordConfig(j)));

  MemoryManager manager(path.c_str(), true);

  BitmapIndex index(&config);
  EXPECT_TRUE(index.init(&manager));

  AnyPostingList pl = index.getPostingList(100);
  EXPECT_TRUE(std::holds_alternative<BitmapPostingList>(pl));
  EXPECT_NE(nullptr, get(pl)->index());

  EXPECT_TRUE(get(pl)->exist(10));
  EXPECT_EQ(1, get(pl)->size());

  AnyPostingListIterator it = get(pl)->iterator();
  EXPECT_EQ(10, get(it)->value()->id);
}
