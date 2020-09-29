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

#include "crystal/storage/index/PostingAllocator.h"

namespace crystal {

PostingAllocator::~PostingAllocator() {
  if (posting_) {
    delete posting_;
  }
  while (head_) {
    MemNode* p = head_->next;
    delete head_;
    head_ = p;
  }
}

bool PostingAllocator::init(Memory* memory, Posting* posting) {
  posting_ = posting;
  return alloc_.init(memory);
}

int64_t PostingAllocator::allocate(size_t size) {
  return alloc_.allocate(size);
}

void PostingAllocator::deallocate(int64_t offset) {
  delayClear();
  alloc_.deallocate(offset);
}

void* PostingAllocator::address(int64_t offset) const {
  return alloc_.address(offset);
}

size_t PostingAllocator::getSize(int64_t offset) const {
  return alloc_.getSize(offset);
}

void PostingAllocator::clear(char* addr) {
  if (posting_) {
    MemNode* node = new MemNode(addr);
    if (head_) {
      tail_->next = node;
    } else {
      head_ = node;
    }
    tail_ = node;
  }
}

void PostingAllocator::delayClear() {
  if (posting_) {
    time_t now = time(nullptr) - kDelaySecond;
    while (head_ && head_->tm <= now) {
      MemNode* p = head_->next;
      posting_->setBase(head_->addr);
      posting_->reset();
      delete head_;
      head_ = p;
    }
    if (head_ == nullptr) {
      tail_ = nullptr;
    }
  }
}

} // namespace crystal
