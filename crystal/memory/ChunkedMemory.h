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

#include <memory>
#include <vector>

#include "crystal/memory/Memory.h"

namespace crystal {

class ChunkedMemory : public Memory {
 public:
  ChunkedMemory(size_t blockCapacity = 4096)
      : blockCapacity_(blockCapacity) {}

  virtual ~ChunkedMemory() {}

  ChunkedMemory(const ChunkedMemory&) = delete;
  ChunkedMemory& operator=(const ChunkedMemory&) = delete;
  ChunkedMemory(ChunkedMemory&&) = default;
  ChunkedMemory& operator=(ChunkedMemory&&) = default;

  bool init() override;

  bool dump() override;

  bool reset() override;

  bool readOnly() const override;

  int64_t allocate(size_t size) override;

  size_t getAllocatedSize() const override;

  void* address(int64_t offset) const override;

  std::string toBuffer() const;

 private:
  struct Block {
    size_t capacity{0};
    size_t used{0};
    int64_t offset{0};
    std::unique_ptr<char[]> buf;

    Block(size_t capacity_, int64_t offset_)
        : capacity(capacity_), offset(offset_) {
      buf.reset(new char[capacity]);
    }

    int64_t current() const {
      return offset + used;
    }
  };

  std::vector<Block> blocks_;
  size_t blockCapacity_;
};

//////////////////////////////////////////////////////////////////////

inline bool ChunkedMemory::readOnly() const {
  return false;
}

inline size_t ChunkedMemory::getAllocatedSize() const {
  return blocks_.empty() ? 0 : blocks_.back().current();
}

} // namespace crystal
