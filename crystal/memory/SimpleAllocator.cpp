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

#include "crystal/memory/SimpleAllocator.h"

namespace crystal {

bool SimpleAllocator::init(Memory* memory) {
  if (!memory) {
    return false;
  }
  memory_ = memory;
  return true;
}

bool SimpleAllocator::reset() {
  return memory_->reset();
}

int64_t SimpleAllocator::allocate(size_t size) {
  if (replace_) {
    if (!memory_->reset()) {
      return 0;
    }
  }
  return allocBuffer(memory_, size);
}

void SimpleAllocator::deallocate(int64_t) {
}

}  // namespace crystal
