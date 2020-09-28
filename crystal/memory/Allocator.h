/*
 * Copyright 2020 Yeolar
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

#include <cstddef>
#include <cstdint>

namespace crystal {

class Allocator {
 public:
  virtual ~Allocator() {}

  virtual int64_t allocate(size_t size) = 0;
  virtual void deallocate(int64_t offset) = 0;

  virtual void* address(int64_t offset) const = 0;
  virtual size_t getSize(int64_t offset) const = 0;
};

}  // namespace crystal
