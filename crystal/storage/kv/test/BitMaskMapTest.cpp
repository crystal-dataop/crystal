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
#include "crystal/storage/kv/BitMaskMap.h"

using namespace crystal;

TEST_F(MMapMemoryTest, BitMaskMap_write) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 100);
  EXPECT_TRUE(memory.init());

  BitMaskMap bmmap(256);
  EXPECT_TRUE(bmmap.init(&memory));

  for (size_t i = 0; i < 256; ++i) {
    EXPECT_TRUE(bmmap.set(i));
    EXPECT_TRUE(bmmap.isSet(i));
    EXPECT_TRUE(bmmap.unset(i));
    EXPECT_FALSE(bmmap.isSet(i));
    EXPECT_TRUE(bmmap.set(i));
  }
  EXPECT_EQ(256, bmmap.slotCount());

  for (size_t i = 256; i < 512; ++i) {
    EXPECT_TRUE(bmmap.set(i));
  }
  EXPECT_EQ(512, bmmap.slotCount());

  for (size_t i = 0; i < 512; ++i) {
    EXPECT_TRUE(bmmap.isSet(i));
  }

  memory.dump();
}

TEST_F(MMapMemoryTest, BitMaskMap_read) {
  MMapMemory memory(path.c_str(), O_RDONLY);
  EXPECT_TRUE(memory.init());

  BitMaskMap bmmap;
  EXPECT_TRUE(bmmap.init(&memory));

  EXPECT_EQ(512, bmmap.slotCount());

  for (size_t i = 0; i < 512; ++i) {
    EXPECT_TRUE(bmmap.isSet(i));
  }
}
