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

#include "crystal/memory/AllocatorUtil.h"
#include "crystal/memory/Memory.h"

namespace crystal {

class FreeStack {
 public:
  FreeStack() {}
  ~FreeStack() {}

  void push(Memory* memory, int64_t offset);
  void pop(Memory* memory);

  int64_t front() const;

 private:
  int64_t& next(Memory* memory, int64_t offset) const;

  int64_t head_{0};
};

int64_t createFreeStackArray(Memory* memory, size_t capacity);

//////////////////////////////////////////////////////////////////////

inline void FreeStack::push(Memory* memory, int64_t offset) {
  next(memory, offset) = head_;
  head_ = offset;
}

inline void FreeStack::pop(Memory* memory) {
  head_ = next(memory, head_);
}

inline int64_t FreeStack::front() const {
  return head_;
}

inline int64_t& FreeStack::next(Memory* memory, int64_t offset) const {
  return *reinterpret_cast<int64_t*>(readBuffer(memory, offset));
}

} // namespace crystal
