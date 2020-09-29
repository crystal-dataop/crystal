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

#include "crystal/storage/kv/FixedChunkMap.h"

namespace crystal {

bool FixedChunkMap::init(Memory* memory) {
  alloc_.init(memory);
  if (memory->getAllocatedSize() == 0) {
    if (alloc_.allocate(0) == 0) {
      return false;
    }
  }
  return true;
}

bool FixedChunkMap::expand(size_t newSize) {
  if (newSize > size()) {
    if (alloc_.allocate(newSize * chunkSize_) == 0) {
      return false;
    }
  }
  return true;
}

}  // namespace crystal
