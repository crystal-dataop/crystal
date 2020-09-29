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

#include "crystal/foundation/BitMask.h"
#include "crystal/memory/SimpleAllocator.h"

namespace crystal {

class BitMaskMap {
 public:
  explicit BitMaskMap(size_t initSlotCount = 256)
      : initSlotCount_(initSlotCount) {}

  virtual ~BitMaskMap() {}

  bool init(Memory* memory);

  void clear();

  bool isSet(uint64_t id) const;

  bool set(uint64_t id);
  bool unset(uint64_t id);

  size_t slotCount() const;

 private:
  bool expand(size_t newSlotCount);

  size_t initSlotCount_;
  SimpleAllocator alloc_{true};
  uint8_t* mapPtr_{nullptr};
  BitMask bitMask_;
};

//////////////////////////////////////////////////////////////////////

inline bool BitMaskMap::isSet(uint64_t id) const {
  return bitMask_.isSet(id);
}

inline size_t BitMaskMap::slotCount() const {
  return alloc_.getSize(kMemStart) * 8;
}

}  // namespace crystal
