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
#include "crystal/storage/index/bitmap/BitmapPosting.h"

namespace crystal {

class BitmapPostingList : public PostingList {
 public:
  struct Meta {
    size_t size{0};
    int64_t offset{0};
    uint64_t maxId{0};
  };

  BitmapPostingList() {}
  BitmapPostingList(IndexBase* index, uint64_t key, const Meta& meta)
      : PostingList(index),
        key_(key),
        meta_(meta) {
  }

  virtual ~BitmapPostingList() {}

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
  friend class BitmapPostingListIterator;

  bool expand(uint64_t maxId);

  bool isSet(uint64_t id) const;
  void set(uint64_t id);
  void unset(uint64_t id);

  uint64_t key_;
  Meta meta_;
};

//////////////////////////////////////////////////////////////////////

inline AnyPosting BitmapPostingList::newPosting() {
  return BitmapPosting();
}

inline AnyPosting BitmapPostingList::getOnlinePosting(uint64_t) {
  return std::monostate();
}

inline bool BitmapPostingList::newPostings(std::vector<AnyPosting>& postings) {
  DCHECK(postings.size() != 0) << "empty postings";
  return true;
}

inline size_t BitmapPostingList::size() const {
  return meta_.size;
}

inline bool BitmapPostingList::exist(uint64_t id) const {
  if (id >= meta_.maxId) {
    return false;
  }
  return isSet(id);
}

}  // namespace crystal
