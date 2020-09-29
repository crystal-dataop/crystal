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

#include <vector>

#include "crystal/storage/index/AnyPosting.h"
#include "crystal/storage/index/AnyPostingListIterator.h"

namespace crystal {

class IndexBase;

class PostingList {
 public:
  PostingList() {}
  PostingList(IndexBase* index)
      : index_(index) {}

  virtual ~PostingList() {}

  IndexBase* index() const {
    return index_;
  }

  virtual AnyPosting newPosting() = 0;
  virtual AnyPosting getOnlinePosting(uint64_t id) = 0;
  virtual AnyPostingListIterator iterator() = 0;
  virtual bool newPostings(std::vector<AnyPosting>& postings) = 0;

  virtual size_t size() const = 0;

  virtual bool exist(uint64_t id) const = 0;

  virtual int add(const Posting& posting) = 0;
  virtual int remove(uint64_t id) = 0;
  virtual int bulkLoad(std::vector<AnyPosting>& postings) = 0;

 protected:
  IndexBase* index_{nullptr};
};

}  // namespace crystal
