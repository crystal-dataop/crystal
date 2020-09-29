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

#include "crystal/storage/index/bitmap/BitmapPostingListIterator.h"

#include "crystal/storage/index/IndexBase.h"
#include "crystal/storage/index/bitmap/BitmapPostingList.h"

namespace crystal {

void BitmapPostingListIterator::seekFirst() {
  auto* addr = reinterpret_cast<uint64_t*>(
      postingList_->index_->allocator().address(postingList_->meta_.offset));
  if (addr == nullptr) {
    return;
  }
  uint64_t n = postingList_->meta_.maxId / 64;
  i = 0;
  while (*addr == 0 && i < n) {
    ++addr;
    ++i;
  }
  if (i < n) {
    j = 0;
    while (j < 64) {
      if (*addr & (uint64_t(1) << j)) {
        break;
      }
      ++j;
    }
    if (j < 64) {
      curPosting_.id = i * 64 + j;
    }
  }
}

void BitmapPostingListIterator::seekLast() {
  auto* addr = reinterpret_cast<uint64_t*>(
      postingList_->index_->allocator().address(postingList_->meta_.offset));
  if (addr == nullptr) {
    return;
  }
  uint64_t n = postingList_->meta_.maxId / 64;
  if (n == 0) {
    return;
  }
  i = n - 1;
  addr += i;
  while (*addr == 0 && i < n) {
    --addr;
    --i;
  }
  if (i < n) {
    j = 63;
    while (j >= 0) {
      if (*addr & (uint64_t(1) << j)) {
        break;
      }
      --j;
    }
    if (j >= 0) {
      curPosting_.id = i * 64 + j;
    }
  }
}

void BitmapPostingListIterator::seekTo(uint64_t id) {
  seekFirst();
  while (isValid() && i * 64 + j < id) {
    next();
  }
  curPosting_.id = i * 64 + j;
}

void BitmapPostingListIterator::next() {
  auto* addr = reinterpret_cast<uint64_t*>(
      postingList_->index_->allocator().address(postingList_->meta_.offset));
  if (addr == nullptr) {
    return;
  }
  ++j;
  while (j < 64) {
    if (addr[i] & (uint64_t(1) << j)) {
      break;
    }
    ++j;
  }
  if (j < 64) {
    curPosting_.id = i * 64 + j;
    return;
  }
  ++i;
  uint64_t n = postingList_->meta_.maxId / 64;
  while (addr[i] == 0 && i < n) {
    ++i;
  }
  if (i < n) {
    j = 0;
    while (j < 64) {
      if (addr[i] & (uint64_t(1) << j)) {
        break;
      }
      ++j;
    }
    if (j < 64) {
      curPosting_.id = i * 64 + j;
    }
  }
}

void BitmapPostingListIterator::prev() {
  auto* addr = reinterpret_cast<uint64_t*>(
      postingList_->index_->allocator().address(postingList_->meta_.offset));
  if (addr == nullptr) {
    return;
  }
  --j;
  while (j >= 0) {
    if (addr[i] & (uint64_t(1) << j)) {
      break;
    }
    --j;
  }
  if (j >= 0) {
    curPosting_.id = i * 64 + j;
    return;
  }
  --i;
  uint64_t n = postingList_->meta_.maxId / 64;
  while (addr[i] == 0 && i < n) {
    --i;
  }
  if (i < n) {
    j = 63;
    while (j >= 0) {
      if (addr[i] & (uint64_t(1) << j)) {
        break;
      }
      --j;
    }
    if (j >= 0) {
      curPosting_.id = i * 64 + j;
    }
  }
}

bool BitmapPostingListIterator::isValid() const {
  return i < postingList_->meta_.maxId / 64 && j >= 0 && j < 64;
}

}  // namespace crystal
