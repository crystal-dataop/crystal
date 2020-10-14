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

#include <cassert>
#include <string>

#include "crystal/memory/MMapFile.h"
#include "crystal/memory/Memory.h"

namespace crystal {

class MMapMemory : public Memory {
 public:
  MMapMemory(const char* name,
             int flags,
             size_t expandSize = 1048576ul,
             size_t maxSize = 1073741824ul * 64);

  virtual ~MMapMemory() {}

  bool init() override;
  bool dump() override;
  bool reset() override;

  bool readOnly() const override;

  int64_t allocate(size_t size) override;
  size_t getAllocatedSize() const override;

  void* address(int64_t offset) const override;

 private:
  bool load();

  bool expand(size_t size);

  std::string name_;
  int flags_;
  size_t expandSize_;
  size_t maxSize_;
  MMapFile data_;
  uint8_t* base_{nullptr};
  Meta meta_;
};

//////////////////////////////////////////////////////////////////////

inline bool MMapMemory::readOnly() const {
  return flags_ == O_RDONLY;
}

inline size_t MMapMemory::getAllocatedSize() const {
  return meta_.allocated;
}

inline void* MMapMemory::address(int64_t offset) const {
  assert(offset >= kMemStart);
  return base_ + offset - kMemStart;
}

}  // namespace crystal
