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

#include <gtest/gtest.h>

#include "crystal/memory/SysAllocator.h"
#include "crystal/serializer/record/Array.h"

using namespace crystal;

TEST(Array, int8) {
  SysAllocator allocator;

  size_t size = 16;
  int64_t offset = allocator.allocate(size * sizeof(int8_t));
  void* ptr = allocator.address(offset);

  Array<int8_t> array(ptr, size);

  EXPECT_EQ(ptr, array.data());
  EXPECT_EQ(size, array.size());

  for (size_t i = 0; i < size; ++i) {
    EXPECT_TRUE(array.set(i, i));
    EXPECT_EQ(i, array.get(i));
  }
}

TEST(Array, compact) {
  SysAllocator allocator;

  size_t size = 16;
  int64_t offset = allocator.allocate(div64(size * 4) * 8);
  void* ptr = allocator.address(offset);

  Array<int8_t> array(ptr, size, 0, 4);

  EXPECT_EQ(ptr, array.data());
  EXPECT_EQ(size, array.size());

  for (size_t i = 0; i < size; ++i) {
    EXPECT_TRUE(array.set(i, i));
    EXPECT_EQ(i, array.get(i));
  }
}

TEST(Array, bool) {
  SysAllocator allocator;

  size_t size = 16;
  int64_t offset = allocator.allocate(div64(size * 1) * 8);
  void* ptr = allocator.address(offset);

  Array<bool> array(ptr, size);

  EXPECT_EQ(ptr, array.data());
  EXPECT_EQ(size, array.size());

  for (size_t i = 0; i < size; ++i) {
    EXPECT_TRUE(array.set(i, i % 2));
    if (i % 2) {
      EXPECT_TRUE(array.get(i));
    } else {
      EXPECT_FALSE(array.get(i));
    }
  }
}

TEST(Array, string) {
  SysAllocator allocator;

  size_t size = 16;
  int64_t offset = allocator.allocate(size * sizeof(int64_t));
  void* ptr = allocator.address(offset);
  memset(ptr, 0, size * sizeof(int64_t));

  Array<std::string_view> array(ptr, size, &allocator);

  EXPECT_EQ(size, array.size());

  for (size_t i = 0; i < size; ++i) {
    EXPECT_TRUE(array.set(i, "string"));
    EXPECT_EQ("string", array.get(i));
  }
}
