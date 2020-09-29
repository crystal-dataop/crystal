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
#include "crystal/storage/kv/FixedChunkMap.h"

using namespace crystal;

TEST_F(MMapMemoryTest, FixedChunkMap_write) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 100);
  EXPECT_TRUE(memory.init());

  FixedChunkMap chunkmap(8);
  EXPECT_TRUE(chunkmap.init(&memory));

  EXPECT_EQ(0, chunkmap.size());
  EXPECT_TRUE(chunkmap.expand(10));
  EXPECT_EQ(10, chunkmap.size());

  for (size_t i = 0; i < 10; ++i) {
    EXPECT_NE(nullptr, chunkmap.getChunk(i));
  }

  memory.dump();
}

TEST_F(MMapMemoryTest, FixedChunkMap_read) {
  MMapMemory memory(path.c_str(), O_RDONLY);
  EXPECT_TRUE(memory.init());

  FixedChunkMap chunkmap(8);
  EXPECT_TRUE(chunkmap.init(&memory));

  EXPECT_EQ(10, chunkmap.size());

  for (size_t i = 0; i < 10; ++i) {
    EXPECT_NE(nullptr, chunkmap.getChunk(i));
  }
}
