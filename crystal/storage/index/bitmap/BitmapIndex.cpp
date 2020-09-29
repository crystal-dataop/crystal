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

#include "crystal/storage/index/bitmap/BitmapIndex.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

bool BitmapIndex::init(MemoryManager* memory) {
  if (!alloc_.init(memory->getMemory(MemoryType::kMemSimple))) {
    CRYSTAL_LOG(ERROR) << "init bitmap allocator failed";
    return false;
  }
  if (!hashMap_.init(memory->getMemory(MemoryType::kMemHash))) {
    CRYSTAL_LOG(ERROR) << "init posting list map failed";
    return false;
  }
  return true;
}

AnyPostingList BitmapIndex::getPostingList(uint64_t key) {
  auto it = hashMap_.find(key);
  if (it == hashMap_.cend()) {
    return std::monostate();
  }
  return BitmapPostingList(this, key, it->second.data);
}

void BitmapIndex::createPostingList(uint64_t key) {
  if (!hashMap_.emplace(key, BitmapPostingList::Meta()).second) {
    CRYSTAL_LOG(WARN) << "posting list with key=" << key << " already exist";
  }
}

void BitmapIndex::updatePostingList(uint64_t key, void* meta) {
  auto it = hashMap_.find(key);
  if (it != hashMap_.cend()) {
    it->second.data = *reinterpret_cast<BitmapPostingList::Meta*>(meta);
  }
}

}  // namespace crystal
