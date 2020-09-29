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

#include "crystal/memory/RecycledAllocator.h"
#include "crystal/memory/test/MMapMemoryTest.h"

using namespace crystal;

TEST_F(MMapMemoryTest, RecycledAllocator_write) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 100);
  EXPECT_TRUE(memory.init());

  RecycledAllocator allocator;
  allocator.init(&memory);

  auto test = [&]() {
    size_t extraSize =
      kMemStart + allocator.getMaxLevel() * 8 + 10000 * 16 + 40 + 40;
    for (size_t i = 1; i <= 256; ++i) {
      allocator.leveling(i);
      int64_t offset = allocator.allocate(i);
      EXPECT_NE(nullptr, allocator.address(offset));
      EXPECT_EQ(i, allocator.getSize(offset));
      EXPECT_EQ(extraSize, offset);
      extraSize += i + sizeof(uint32_t);
    }
    int64_t offset =
      kMemStart + allocator.getMaxLevel() * 8 + 10000 * 16 + 40 + 40;
    for (size_t i = 1; i <= 256; ++i) {
      allocator.leveling(i);
      if (i % 2) {
        allocator.deallocate(offset);
      }
      offset += i + sizeof(uint32_t);
    }
  };

  test();
  allocator.reset();
  test();

  memory.dump();
}

TEST_F(MMapMemoryTest, RecycledAllocator_read) {
  MMapMemory memory(path.c_str(), O_RDONLY);
  EXPECT_TRUE(memory.init());

  RecycledAllocator allocator;
  allocator.init(&memory);

  auto test = [&]() {
    int64_t offset =
      kMemStart + allocator.getMaxLevel() * 8 + 10000 * 16 + 40 + 40;
    for (size_t i = 1; i <= 256; ++i) {
      allocator.leveling(i);
      EXPECT_NE(nullptr, allocator.address(offset));
      EXPECT_EQ(i, allocator.getSize(offset));
      offset += i + sizeof(uint32_t);
    }
  };

  test();
}
