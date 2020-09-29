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

#include "crystal/memory/test/MMapMemoryTest.h"
#include "crystal/storage/kv/HashMap.h"

using namespace crystal;

TEST_F(MMapMemoryTest, HashMap_write) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 100);
  EXPECT_TRUE(memory.init());

  HashMap<uint64_t, uint32_t> hashmap(10000);
  EXPECT_TRUE(hashmap.init(&memory));

  for (size_t i = 0; i < 10000; ++i) {
    auto p = hashmap.emplace(i, i * 100);
    EXPECT_EQ(i, p.first->first);
    EXPECT_EQ(i * 100, p.first->second.data);
    EXPECT_TRUE(p.second);
  }
  auto it1 = hashmap.cbegin();
  EXPECT_NE(hashmap.cend(), it1);
  auto it2 = hashmap.find(1000);
  EXPECT_EQ(1000, it2->first);
  EXPECT_EQ(100000, it2->second.data);

  memory.dump();
}

TEST_F(MMapMemoryTest, HashMap_read) {
  MMapMemory memory(path.c_str(), O_RDONLY);
  EXPECT_TRUE(memory.init());

  HashMap<uint64_t, uint32_t> hashmap(10000);
  EXPECT_TRUE(hashmap.init(&memory));

  auto it1 = hashmap.cbegin();
  EXPECT_NE(hashmap.cend(), it1);
  auto it2 = hashmap.find(1000);
  EXPECT_EQ(1000, it2->first);
  EXPECT_EQ(100000, it2->second.data);
}
