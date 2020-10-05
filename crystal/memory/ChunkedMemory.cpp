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

#include "crystal/memory/ChunkedMemory.h"

namespace crystal {

bool ChunkedMemory::init() {
  return true;
}

bool ChunkedMemory::dump() {
  return true;
}

bool ChunkedMemory::reset() {
  blocks_.clear();
  return true;
}

int64_t ChunkedMemory::allocate(size_t size) {
  if (blocks_.empty() || size > blocks_.back().capacity) {
    blocks_.emplace_back(std::max(size, blockCapacity_), getAllocatedSize());
  }
  int64_t offset = blocks_.back().current();
  blocks_.back().capacity -= size;
  blocks_.back().used += size;
  return ++offset;
}

void* ChunkedMemory::address(int64_t offset) const {
  if (offset > 0) {
    --offset;
    for (auto& block : blocks_) {
      if (offset < block.current()) {
        return block.buf.get() + (offset - block.offset);
      }
    }
  }
  return nullptr;
}

std::string ChunkedMemory::toBuffer() const {
  std::string buffer;
  buffer.reserve(getAllocatedSize());
  for (auto& block : blocks_) {
    buffer.append(block.buf.get(), block.used);
  }
  return buffer;
}

} // namespace crystal
