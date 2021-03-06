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

#include "crystal/memory/SimpleAllocator.h"

namespace crystal {

class FixedChunkMap {
 public:
  explicit FixedChunkMap(size_t chunkSize)
      : chunkSize_(chunkSize) {}

  virtual ~FixedChunkMap() {}

  bool init(Memory* memory);

  void* getChunk(uint64_t id) const;

  size_t size() const;

  bool expand(size_t size);

 private:
  size_t chunkSize_;
  SimpleAllocator alloc_{true};
};

//////////////////////////////////////////////////////////////////////

inline void* FixedChunkMap::getChunk(uint64_t id) const {
  return reinterpret_cast<uint8_t*>(alloc_.address(kMemStart))
      + chunkSize_ * id;
}

inline size_t FixedChunkMap::size() const {
  return alloc_.getSize(kMemStart) / chunkSize_;
}

}  // namespace crystal
