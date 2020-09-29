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

#include "crystal/memory/detail/FreeStack.h"

#include <memory>

namespace crystal {

int64_t createFreeStackArray(Memory* memory, size_t n) {
  int64_t offset = memory->allocate(sizeof(FreeStack) * n);
  if (offset > 0) {
    FreeStack* p = address<FreeStack>(memory, offset);
    for (size_t i = 0; i < n; ++i) {
      new (p + i) FreeStack();
    }
  }
  return offset;
}

} // namespace crystal
