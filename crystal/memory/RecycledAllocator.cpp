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

#include <ctime>

#include "crystal/foundation/Logging.h"

namespace crystal {

RecycledAllocator::RecycledAllocator(
    size_t minMemSize,
    size_t maxMemSize,
    float rate,
    float expandFactor,
    uint32_t delayTime,
    size_t delayQueueSize) {
  meta_.minMemSize = std::max(minMemSize, sizeof(int64_t));
  meta_.maxMemSize = maxMemSize;
  meta_.rate = std::max(rate, 1.05f);
  meta_.level = leveling(meta_.maxMemSize) + 1;
  meta_.expandFactor = expandFactor;
  meta_.delayTime = delayTime;
  meta_.delayQueueSize = delayQueueSize;
}

bool RecycledAllocator::init(Memory* memory) {
  if (!memory) {
    return false;
  }
  memory_ = memory;
  if (memory_->getAllocatedSize() != 0) {
    meta_ = *crystal::address<Meta>(memory_, kMemStart);
    freeStackOffset_ = kMemStart + sizeof(Meta);
    delayQueueOffset_ = freeStackOffset_ + sizeof(FreeStack) * meta_.level;
    return true;
  } else {
    return reset();
  }
}

bool RecycledAllocator::reset() {
  if (!memory_->reset()) {
    return false;
  }
  if (write(memory_, meta_) == 0) {
    return false;
  }
  freeStackOffset_ = createFreeStackArray(memory_, meta_.level);
  if (freeStackOffset_ == 0) {
    return false;
  }
  delayQueueOffset_ = createQueue<QueueNode>(memory_, meta_.delayQueueSize);
  if (delayQueueOffset_ == 0) {
    return false;
  }
  return true;
}

int64_t RecycledAllocator::allocate(size_t size) {
  if (size == 0 || size > meta_.maxMemSize) {
    return 0;
  }
  int64_t offset = 0;
  size_t n = size;
  size_t a = leveling(n);
  size_t m = std::min(size_t(n * meta_.expandFactor), meta_.maxMemSize);
  size_t b = leveling(m);
  FreeStack* stack = crystal::address<FreeStack>(memory_, freeStackOffset_);
  for (size_t i = a; i <= b; ++i) {
    if (stack[i].front() != 0) {
      offset = stack[i].front();
      stack[i].pop(memory_);
      break;
    }
  }
  if (offset == 0) {
    offset = allocBuffer(memory_, n);
  }
  return offset;
}

void RecycledAllocator::deallocate(int64_t offset) {
  if (offset <= 0) {
    CRYSTAL_LOG(ERROR) << "invalid offset";
    return;
  }
  freeQueue();
  DelayQueue* queue = crystal::address<DelayQueue>(memory_, delayQueueOffset_);
  if (queue->full()) {
    queue = expandQueue<QueueNode>(memory_,
                                   delayQueueOffset_,
                                   meta_.delayQueueSize);
    if (!queue) {
      CRYSTAL_LOG(ERROR) << "expand delay queue failed";
      return;
    }
  }
  size_t size = getSize(offset);
  QueueNode node = {
    offset,
    leveling(size),
    uint32_t(time(nullptr))
  };
  queue->push(memory_, node);
}

void RecycledAllocator::freeQueue() {
  uint32_t now = uint32_t(time(nullptr));
  FreeStack* stack = crystal::address<FreeStack>(memory_, freeStackOffset_);
  DelayQueue* queue = crystal::address<DelayQueue>(memory_, delayQueueOffset_);
  while (!queue->empty()) {
    QueueNode node = queue->front(memory_);
    if (node.time + meta_.delayTime >= now) {
      break;
    }
    stack[node.level].push(memory_, node.offset);
    queue->pop();
  }
}

int RecycledAllocator::leveling(size_t& size) const {
  int level = 0;
  size_t n = meta_.minMemSize;
  while (n < size) {
    n = std::max(size_t(n * meta_.rate), n + 1);
    ++level;
  }
  size = n;
  return level;
}

}  // namespace crystal
