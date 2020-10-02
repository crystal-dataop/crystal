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

#include "crystal/storage/index/PostingList.h"
#include "crystal/storage/index/vector/VectorPosting.h"

namespace crystal {

struct VectorData {
  std::vector<float> data;
  int64_t dimension;
  uint64_t firstId{0};
};

class VectorPostingList : public PostingList {
 public:
  VectorPostingList() {}
  VectorPostingList(IndexBase* index, VectorData* vectors)
      : PostingList(index), vectors_(vectors) {
  }

  virtual ~VectorPostingList() {}

  AnyPosting newPosting() override;
  AnyPosting getOnlinePosting(uint64_t id) override;
  AnyPostingListIterator iterator() override;
  bool newPostings(std::vector<AnyPosting>& postings) override;

  size_t size() const override;

  bool exist(uint64_t id) const override;

  int add(const Posting& posting) override;
  int remove(uint64_t id) override;
  int bulkLoad(std::vector<AnyPosting>& postings) override;

 private:
  friend class VectorPostingListIterator;

  VectorData* vectors_;
};

//////////////////////////////////////////////////////////////////////

inline AnyPosting VectorPostingList::getOnlinePosting(uint64_t) {
  return std::monostate();
}

inline size_t VectorPostingList::size() const {
  return vectors_->data.size() / vectors_->dimension;
}

inline bool VectorPostingList::exist(uint64_t id) const {
  return id < vectors_->firstId + size();
}

}  // namespace crystal
