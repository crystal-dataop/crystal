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

#include "crystal/storage/index/PostingListIterator.h"
#include "crystal/storage/index/vector/VectorPosting.h"

namespace crystal {

struct VectorData;
class VectorPostingList;

class VectorPostingListIterator : public PostingListIterator {
 public:
  VectorPostingListIterator() {}
  explicit VectorPostingListIterator(VectorPostingList* postingList);

  virtual ~VectorPostingListIterator() {}

  VectorPostingListIterator(VectorPostingListIterator&&) = default;
  VectorPostingListIterator& operator=(VectorPostingListIterator&&) = default;

  void seekFirst() override;
  void seekLast() override;
  void seekTo(uint64_t id) override;

  void next() override;
  void prev() override;

  bool isValid() const override;

  const Posting* value() const override;

 private:
  VectorPostingList* postingList_;
  VectorPosting curPosting_;
  VectorData* vectors_;
  size_t i_{0};
  size_t postingSize_{0};
};

//////////////////////////////////////////////////////////////////////

inline const Posting* VectorPostingListIterator::value() const {
  return isValid() ? &curPosting_ : nullptr;
}

}  // namespace crystal
