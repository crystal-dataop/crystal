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

#include "crystal/storage/index/Posting.h"

namespace crystal {

class IndexBase;
class PostingList;

class VectorPosting : public Posting {
 public:
  VectorPosting() {}
  VectorPosting(PostingList* postingList, bool ownMemory, int64_t dimension);

  virtual ~VectorPosting();

  VectorPosting(const VectorPosting&) = delete;
  VectorPosting& operator=(const VectorPosting&) = delete;
  VectorPosting(VectorPosting&& other);
  VectorPosting& operator=(VectorPosting&& other);

  void setBase(char* base) override;

  const char* data() const override;
  size_t size() const override;

  bool parseFrom(const Record& record) override;
  bool update(const Record& record) override;

 private:
  PostingList* postingList_{nullptr};
  char* base_{nullptr};
  bool ownMemory_{false};
  size_t size_{0};
};

//////////////////////////////////////////////////////////////////////

inline const char* VectorPosting::data() const {
  return base_;
}

inline size_t VectorPosting::size() const {
  return size_;
}

}  // namespace crystal
