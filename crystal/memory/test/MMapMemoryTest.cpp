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
#include "crystal/memory/test/MMapMemoryTest.h"

using namespace crystal;

TEST_F(MMapMemoryTest, write) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 1024);
  EXPECT_TRUE(memory.init());

  int i = 100;
  EXPECT_EQ(kMemStart, write(&memory, i));

  EXPECT_EQ(sizeof(int), memory.getAllocatedSize());
  EXPECT_TRUE(memory.dump());
}

TEST_F(MMapMemoryTest, read) {
  MMapMemory memory(path.c_str(), O_RDONLY);
  EXPECT_TRUE(memory.init());

  int* p = address<int>(&memory, kMemStart);
  EXPECT_NE(nullptr, p);
  EXPECT_EQ(100, *p);

  EXPECT_EQ(sizeof(int), memory.getAllocatedSize());
  EXPECT_FALSE(memory.dump());
}
