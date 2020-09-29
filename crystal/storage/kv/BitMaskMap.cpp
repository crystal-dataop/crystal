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

#include "crystal/storage/kv/BitMaskMap.h"

#include "crystal/math/Div.h"

namespace crystal {

bool BitMaskMap::init(Memory* memory) {
  alloc_.init(memory);
  if (memory->getAllocatedSize() == 0) {
    if (alloc_.allocate(div8(initSlotCount_)) == 0) {
      return false;
    }
    memset(alloc_.address(kMemStart), 0, alloc_.getSize(kMemStart));
  }
  mapPtr_ = reinterpret_cast<uint8_t*>(alloc_.address(kMemStart));
  bitMask_.reset(mapPtr_);
  return true;
}

void BitMaskMap::clear() {
  memset(mapPtr_, 0, alloc_.getSize(kMemStart));
}

bool BitMaskMap::set(uint64_t id) {
  if (id >= slotCount()) {
    if (!expand(id + 1)) {
      return false;
    }
  }
  bitMask_.set(id);
  return true;
}

bool BitMaskMap::unset(uint64_t id) {
  if (id >= slotCount()) {
    if (!expand(id + 1)) {
      return false;
    }
  }
  bitMask_.unset(id);
  return true;
}

bool BitMaskMap::expand(size_t newSlotCount) {
  size_t oldSize = div8(slotCount());
  size_t newSize = div8(newSlotCount);
  if (newSize > oldSize) {
    if (alloc_.allocate(newSize) == 0) {
      return false;
    }
    memset(mapPtr_ + oldSize, 0, newSize - oldSize);
  }
  return true;
}

}  // namespace crystal
