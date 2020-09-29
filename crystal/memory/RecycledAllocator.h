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

#pragma once

#include "crystal/memory/Allocator.h"
#include "crystal/memory/detail/FreeStack.h"
#include "crystal/memory/detail/Queue.h"

namespace crystal {

class RecycledAllocator : public Allocator {
 public:
  RecycledAllocator(size_t minMemSize = sizeof(int64_t),
                    size_t maxMemSize = 1ul << 31,
                    float rate = 1.05,
                    float expandFactor = 2.0,
                    uint32_t delayTime = 10,
                    size_t delayQueueSize = 10000);

  virtual ~RecycledAllocator() {}

  bool init(Memory* memory);

  bool reset();

  int64_t allocate(size_t size) override;
  void deallocate(int64_t offset) override;

  void* address(int64_t offset) const override;
  size_t getSize(int64_t offset) const override;

  Memory* getMemory();

  int leveling(size_t& size) const;
  int getMaxLevel() const;

 private:
  struct QueueNode {
    int64_t offset;
    int level;
    uint32_t time;
  };

  typedef Queue<QueueNode> DelayQueue;

  void freeQueue();

  struct Meta {
    size_t minMemSize;
    size_t maxMemSize;
    float rate;
    int level;
    float expandFactor;
    uint32_t delayTime;
    size_t delayQueueSize;
  };

  Meta meta_;
  int64_t freeStackOffset_{0};
  int64_t delayQueueOffset_{0};
  Memory* memory_{nullptr};
};

//////////////////////////////////////////////////////////////////////

inline void* RecycledAllocator::address(int64_t offset) const {
  return readBuffer(memory_, offset);
}

inline size_t RecycledAllocator::getSize(int64_t offset) const {
  return getBufferSize(memory_, offset);
}

inline Memory* RecycledAllocator::getMemory() {
  return memory_;
}

inline int RecycledAllocator::getMaxLevel() const {
  return meta_.level;
}

}  // namespace crystal
