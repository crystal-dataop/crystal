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

#include "crystal/memory/detail/FreeStack.h"
#include "crystal/memory/test/MMapMemoryTest.h"

using namespace crystal;

TEST_F(MMapMemoryTest, FreeStack) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 100);
  EXPECT_TRUE(memory.init());

  int64_t offset = createFreeStackArray(&memory, 1);
  FreeStack* fstack = address<FreeStack>(&memory, offset);

  EXPECT_EQ(0, fstack->front());

  for (int i = 0; i < 10; ++i) {
    offset = allocBuffer(&memory, sizeof(int64_t));
    fstack->push(&memory, offset);
    EXPECT_EQ(offset, fstack->front());
  }
  for (int i = 0; i < 10; ++i) {
    fstack->pop(&memory);
  }
  EXPECT_EQ(0, fstack->front());

  memory.dump();
}
