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

#include "crystal/storage/index/vector/VectorPosting.h"
#include "crystal/storage/index/IndexBase.h"

namespace crystal {

VectorPosting::VectorPosting(
    PostingList* postingList, bool ownMemory, int64_t dimension)
    : postingList_(postingList),
      ownMemory_(ownMemory),
      size_(dimension * sizeof(float)) {
}

VectorPosting::~VectorPosting() {
  if (ownMemory_) {
    free(base_);
  }
}

VectorPosting::VectorPosting(VectorPosting&& other) {
  *this = std::move(other);
}

VectorPosting& VectorPosting::operator=(VectorPosting&& other) {
  std::swap(postingList_, other.postingList_);
  std::swap(base_, other.base_);
  std::swap(ownMemory_, other.ownMemory_);
  std::swap(size_, other.size_);
  return *this;
}

void VectorPosting::setBase(char* base) {
  base_ = base;
}

bool VectorPosting::parseFrom(const Record& record) {
  const FieldMeta* meta = record.recordMeta()->getMeta("__id");
  if (meta == nullptr) {
    CRYSTAL_LOG(ERROR) << "__id field not found";
    return false;
  }
  id = record.get<uint64_t>(*meta);
  auto a = record.get<Array<float>>(postingList_->index()->keyMeta());
  if (a.size() * sizeof(float) != size_) {
    CRYSTAL_LOG(ERROR) << "vector data unmatch dimension";
    return false;
  }
  memcpy(base_, a.data(), size_);
  return true;
}

bool VectorPosting::update(const Record&) {
  CRYSTAL_LOG(ERROR) << "update not support";
  return false;
}

}  // namespace crystal
