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

#include "crystal/memory/detail/Queue.h"
#include "crystal/memory/test/MMapMemoryTest.h"

using namespace crystal;

TEST_F(MMapMemoryTest, Queue) {
  MMapMemory memory(path.c_str(), O_RDWR | O_CREAT, 100);
  EXPECT_TRUE(memory.init());

  int64_t offset = createQueue<int>(&memory, 10);
  Queue<int>* queue = address<Queue<int>>(&memory, offset);

  EXPECT_TRUE(queue->empty());

  for (int i = 0; i < 10; ++i) {
    EXPECT_TRUE(queue->push(&memory, i));
  }
  EXPECT_TRUE(queue->full());

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(i, queue->front(&memory));
    EXPECT_TRUE(queue->pop());
  }
  EXPECT_TRUE(queue->empty());

  memory.dump();
}
