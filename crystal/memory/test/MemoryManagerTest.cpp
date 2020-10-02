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

#include "crystal/memory/AllocatorUtil.h"
#include "crystal/memory/test/MemoryManagerTest.h"

using namespace crystal;

TEST_F(MemoryManagerTest, write) {
  MemoryManager manager(path.c_str(), false);

  for (int i = 0; i < kMemMax; ++i) {
    if (i != kMemFaiss) {
      Memory* memory = manager.getMemory(i);
      int v = 100;
      EXPECT_EQ(kMemStart, write(memory, v));
      EXPECT_EQ(sizeof(int), memory->getAllocatedSize());
    }
  }
  manager.dump();
}

TEST_F(MemoryManagerTest, read) {
  MemoryManager manager(path.c_str(), true);

  for (int i = 0; i < kMemMax; ++i) {
    if (i != kMemFaiss) {
      Memory* memory = manager.getMemory(i);
      int* p = address<int>(memory, kMemStart);
      EXPECT_NE(nullptr, p);
      EXPECT_EQ(100, *p);
      EXPECT_EQ(sizeof(int), memory->getAllocatedSize());
    }
  }
}
