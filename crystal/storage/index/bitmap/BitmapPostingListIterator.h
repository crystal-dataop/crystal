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
#include "crystal/storage/index/bitmap/BitmapPosting.h"

namespace crystal {

class BitmapPostingList;

class BitmapPostingListIterator : public PostingListIterator {
 public:
  BitmapPostingListIterator() {}
  explicit BitmapPostingListIterator(BitmapPostingList* postingList)
      : postingList_(postingList) {}

  virtual ~BitmapPostingListIterator() {}

  void seekFirst() override;
  void seekLast() override;
  void seekTo(uint64_t id) override;

  void next() override;
  void prev() override;

  bool isValid() const override;

  const Posting* value() const override;

 private:
  BitmapPostingList* postingList_;
  BitmapPosting curPosting_;
  uint64_t i{0};  // index for uint64_t id
  int j{0};       // index in uint64_t id bits
};

//////////////////////////////////////////////////////////////////////

inline const Posting* BitmapPostingListIterator::value() const {
  return isValid() ? &curPosting_ : nullptr;
}

}  // namespace crystal
