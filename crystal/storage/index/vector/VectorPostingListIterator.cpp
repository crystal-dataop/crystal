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

#include "crystal/storage/index/vector/VectorPostingListIterator.h"
#include "crystal/storage/index/vector/VectorPostingList.h"

namespace crystal {

VectorPostingListIterator::VectorPostingListIterator(
    VectorPostingList* postingList)
    : postingList_(postingList),
      curPosting_(postingList_, false, postingList_->vectors_->dimension),
      vectors_(postingList_->vectors_),
      postingSize_(curPosting_.size()) {}

void VectorPostingListIterator::seekFirst() {
  i_ = 0;
  float* addr = &vectors_->data[0];
  curPosting_.setBase(reinterpret_cast<char*>(addr));
  curPosting_.id = vectors_->firstId;
}

void VectorPostingListIterator::seekLast() {
  i_ = postingList_->size() - 1;
  float* addr = &vectors_->data[postingSize_ / sizeof(float) * i_];
  curPosting_.setBase(reinterpret_cast<char*>(addr));
  curPosting_.id = vectors_->firstId + i_;
}

void VectorPostingListIterator::seekTo(uint64_t id) {
  seekFirst();
  while (isValid() && curPosting_.id != id) {
    next();
  }
}

void VectorPostingListIterator::next() {
  ++i_;
  if (i_ >= postingList_->size()) {
    return;
  }
  float* addr = &vectors_->data[postingSize_ / sizeof(float) * i_];
  curPosting_.setBase(reinterpret_cast<char*>(addr));
  curPosting_.id = vectors_->firstId + i_;
}

void VectorPostingListIterator::prev() {
  --i_;
  if (i_ >= postingList_->size()) {
    return;
  }
  float* addr = &vectors_->data[postingSize_ / sizeof(float) * i_];
  curPosting_.setBase(reinterpret_cast<char*>(addr));
  curPosting_.id = vectors_->firstId + i_;
}

bool VectorPostingListIterator::isValid() const {
  return curPosting_.data() != nullptr && i_ < postingList_->size();
}

}  // namespace crystal
