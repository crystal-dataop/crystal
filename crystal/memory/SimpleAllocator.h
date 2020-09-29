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

#include "crystal/memory/Allocator.h"
#include "crystal/memory/AllocatorUtil.h"

namespace crystal {

class SimpleAllocator : public Allocator {
 public:
  SimpleAllocator(bool replace = false)
      : replace_(replace) {}

  virtual ~SimpleAllocator() {}

  bool init(Memory* memory);

  bool reset();

  int64_t allocate(size_t size) override;
  void deallocate(int64_t offset) override;

  void* address(int64_t offset) const override;
  size_t getSize(int64_t offset) const override;

  Memory* getMemory();

 private:
  Memory* memory_{nullptr};
  bool replace_;
};

//////////////////////////////////////////////////////////////////////

inline void* SimpleAllocator::address(int64_t offset) const {
  return readBuffer(memory_, offset);
}

inline size_t SimpleAllocator::getSize(int64_t offset) const {
  return getBufferSize(memory_, offset);
}

inline Memory* SimpleAllocator::getMemory() {
  return memory_;
}

}  // namespace crystal
