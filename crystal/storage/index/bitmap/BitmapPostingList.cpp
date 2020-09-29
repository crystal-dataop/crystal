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

#include "crystal/storage/index/bitmap/BitmapPostingList.h"

#include "crystal/foundation/Logging.h"
#include "crystal/storage/index/IndexBase.h"

namespace crystal {

constexpr uint64_t kDefaultMaxId = 1 << 20;

bool BitmapPostingList::expand(uint64_t maxId) {
  if (maxId < kDefaultMaxId) {
    maxId = kDefaultMaxId;
  }
  uint64_t n = div64(maxId);
  int64_t offset = index_->allocator().allocate(n * sizeof(uint64_t));
  if (offset == 0) {
    CRYSTAL_LOG(ERROR) << "allocate failed";
    return false;
  }
  void* dst = index_->allocator().address(offset);
  memset(dst, 0, n * sizeof(uint64_t));
  if (meta_.offset != 0) {
    void* src = index_->allocator().address(meta_.offset);
    memcpy(dst, src, meta_.maxId / 8);
    index_->allocator().deallocate(meta_.offset);
  }
  meta_.offset = offset;
  meta_.maxId = n * 64;
  index_->updatePostingList(key_, &meta_);
  return true;
}

bool BitmapPostingList::isSet(uint64_t id) const {
  auto* addr = reinterpret_cast<uint64_t*>(
      index_->allocator().address(meta_.offset));
  if (addr == nullptr) {
    CRYSTAL_LOG(ERROR) << "get address failed";
    return false;
  }
  return addr[id / 64] & (uint64_t(1) << (id % 64));
}

void BitmapPostingList::set(uint64_t id) {
  auto* addr = reinterpret_cast<uint64_t*>(
      index_->allocator().address(meta_.offset));
  if (addr == nullptr) {
    CRYSTAL_LOG(ERROR) << "get address failed";
    return;
  }
  addr[id / 64] |= uint64_t(1) << (id % 64);
}

void BitmapPostingList::unset(uint64_t id) {
  auto* addr = reinterpret_cast<uint64_t*>(
      index_->allocator().address(meta_.offset));
  if (addr == nullptr) {
    CRYSTAL_LOG(ERROR) << "get address failed";
    return;
  }
  addr[id / 64] &= ~(uint64_t(1) << (id % 64));
}

AnyPostingListIterator BitmapPostingList::iterator() {
  if (!index_) {
    return std::monostate();
  }
  BitmapPostingListIterator it(this);
  it.seekFirst();
  return it;
}

int BitmapPostingList::add(const Posting& posting) {
  uint64_t id = posting.id;
  if (id >= meta_.maxId) {
    if (!expand(id + 1)) {
      CRYSTAL_LOG(ERROR) << "expand bitmap index failed";
      return -1;
    }
  }
  if (!isSet(id)) {
    set(id);
    ++meta_.size;
    index_->updatePostingList(key_, &meta_);
  }
  return 0;
}

int BitmapPostingList::remove(uint64_t id) {
  if (id >= meta_.maxId) {
    CRYSTAL_LOG(ERROR) << "id not exist";
    return -1;
  }
  if (isSet(id)) {
    unset(id);
    --meta_.size;
    index_->updatePostingList(key_, &meta_);
  }
  return 0;
}

int BitmapPostingList::bulkLoad(std::vector<AnyPosting>& postings) {
  uint64_t maxId = meta_.maxId;
  for (auto& posting : postings) {
    uint64_t id = get(posting)->id;
    if (id >= maxId) {
      maxId = id + 1;
    }
  }
  if (!expand(maxId)) {
    CRYSTAL_LOG(ERROR) << "expand bitmap index failed";
    return -1;
  }
  for (auto& posting : postings) {
    set(get(posting)->id);
  }
  meta_.size = postings.size();
  index_->updatePostingList(key_, &meta_);
  return 0;
}

}  // namespace crystal
