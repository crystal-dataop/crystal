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

#include "crystal/memory/SysAllocator.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

bool SysAllocator::init(Memory*) {
  return true;
}

int64_t SysAllocator::SysMemory::allocate(size_t size) {
  return (int64_t) malloc(size);
}

void SysAllocator::SysMemory::deallocate(int64_t offset) {
  free(address(offset));
}

void* SysAllocator::SysMemory::address(int64_t offset) const {
  return (void*) offset;
}

int64_t SysAllocator::allocate(size_t size) {
  if (size == 0) {
    return 0;
  }
  return allocBuffer(&memory_, size);
}

void SysAllocator::deallocate(int64_t offset) {
  if (offset == 0) {
    CRYSTAL_LOG(ERROR) << "invalid offset";
    return;
  }
  memory_.deallocate(offset);
}

}  // namespace crystal
