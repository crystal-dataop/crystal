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

#include <ctime>

#include "crystal/memory/RecycledAllocator.h"
#include "crystal/storage/index/Posting.h"

namespace crystal {

class PostingAllocator : public Allocator {
 public:
  PostingAllocator() {}
  virtual ~PostingAllocator();

  bool init(Memory* memory, Posting* posting = nullptr);

  int64_t allocate(size_t size) override;
  void deallocate(int64_t offset) override;

  void* address(int64_t offset) const override;
  size_t getSize(int64_t offset) const override;

  void clear(char* addr);

 private:
  static constexpr int kDelaySecond = 3;

  void delayClear();

  struct MemNode {
    char* addr;
    time_t tm{time(nullptr)};
    MemNode* next{nullptr};

    MemNode(char* p) : addr(p) {}
  };

  RecycledAllocator alloc_;
  Posting* posting_;
  MemNode* head_{nullptr};
  MemNode* tail_{nullptr};
};

} // namespace crystal
