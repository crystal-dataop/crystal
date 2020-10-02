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

#include "crystal/storage/index/vector/VectorPostingList.h"

#include "crystal/foundation/Logging.h"
#include "crystal/storage/index/IndexBase.h"
#include "crystal/storage/index/vector/VectorIndex.h"

namespace crystal {

AnyPosting VectorPostingList::newPosting() {
  VectorPosting posting(this, true, vectors_->dimension);
  char* addr = reinterpret_cast<char*>(malloc(posting.size()));
  if (addr == nullptr) {
    return std::monostate();
  }
  posting.setBase(addr);
  return std::move(posting);
}

AnyPostingListIterator VectorPostingList::iterator() {
  VectorPostingListIterator it(this);
  it.seekFirst();
  return std::move(it);
}

bool VectorPostingList::newPostings(std::vector<AnyPosting>& postings) {
  size_t size = postings.size();
  DCHECK(size != 0) << "empty postings";
  VectorPosting tmp(this, false, vectors_->dimension);
  char* addr = reinterpret_cast<char*>(malloc(tmp.size() * size));
  if (addr == nullptr) {
    return false;
  }
  for (size_t i = 0; i < size; ++i) {
    VectorPosting posting(this, i == 0, vectors_->dimension);
    posting.setBase(addr + tmp.size() * i);
    postings[i] = std::move(posting);
  }
  return true;
}

int VectorPostingList::add(const Posting& posting) {
  auto p = reinterpret_cast<const float*>(posting.data());
  auto e = reinterpret_cast<const float*>(posting.data() + posting.size());
  vectors_->data.insert(vectors_->data.end(), p, e);
  reinterpret_cast<VectorIndex*>(index_)->vadd();
  return 0;
}

int VectorPostingList::remove(uint64_t) {
  CRYSTAL_LOG(ERROR) << "remove not support";
  return -1;
}

int VectorPostingList::bulkLoad(std::vector<AnyPosting>& postings) {
  auto posting = get(postings[0]);
  auto p = reinterpret_cast<const float*>(posting->data());
  auto e = reinterpret_cast<const float*>(posting->data() + posting->size());
  vectors_->data.insert(vectors_->data.end(), p, e);
  reinterpret_cast<VectorIndex*>(index_)->vadd(true);
  return 0;
}

}  // namespace crystal
