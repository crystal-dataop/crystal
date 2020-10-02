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
#include "crystal/memory/test/MemoryManagerTest.h"
#include "crystal/storage/kv/KV.h"

using namespace crystal;

class KVTest : public MemoryManagerTest {
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
        key="menuId",
        value=["menuId", "status", "content", "food"],
        bucket=10000
      }
      )";
};

TEST_F(KVTest, write) {
  KVConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j, parseRecordConfig(j)));

  MemoryManager manager(path.c_str(), false);

  KV kv(config);
  EXPECT_TRUE(kv.init(&manager));

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_EQ(-1, kv.find(i));
    EXPECT_TRUE(kv.insert(i, i));
    EXPECT_EQ(i, kv.find(i));
  }

  Accessor accessor(kv.recordMeta());
  SysAllocator alloc;

  Record record;
  record.init(&kv.recordMeta(), &accessor, &alloc);

  void* buf = alloc.address(alloc.allocate(accessor.bufferSize()));
  memset(buf, 0, accessor.bufferSize());
  record.setBuffer(buf);
  record.reset();

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_FALSE(kv.exist(i));
    EXPECT_TRUE(kv.add(i, record));
    EXPECT_TRUE(kv.exist(i));
    if (i > 100) {
      EXPECT_TRUE(kv.remove(i));
      EXPECT_FALSE(kv.exist(i));
    }
  }

  manager.dump();
}

TEST_F(KVTest, read) {
  KVConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j, parseRecordConfig(j)));

  MemoryManager manager(path.c_str(), true);

  KV kv(config);
  EXPECT_TRUE(kv.init(&manager));

  for (uint64_t i = 1; i <= 10000; i *= 10) {
    EXPECT_EQ(i, kv.find(i));
    if (i > 100) {
      EXPECT_FALSE(kv.exist(i));
    } else {
      EXPECT_TRUE(kv.exist(i));
      void* ptr = kv.getRecordPtr(i);
      EXPECT_NE(nullptr, ptr);
      Record record = kv.createRecord(ptr);
      EXPECT_EQ(1, record.get<uint32_t>(
              *record.recordMeta()->getMeta("status")));
    }
  }
}
