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

#include "crystal/memory/AllocatorUtil.h"
#include "crystal/memory/ChunkedMemory.h"

using namespace crystal;

TEST(ChunkedMemory, all) {
  ChunkedMemory memory(16);
  EXPECT_TRUE(memory.init());

  for (size_t i = 0; i < 20; i++) {
    char c = 97 + i;
    EXPECT_EQ(kMemStart + i, write(&memory, c));
  }

  EXPECT_EQ(20, memory.getAllocatedSize());
  EXPECT_STREQ("abcdefghijklmnopqrst", memory.toBuffer().c_str());
  EXPECT_TRUE(memory.dump());
}
