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

#include <cstddef>
#include <cstdint>
#include <string>

namespace crystal {

constexpr int64_t kMemStart = 1;

class Memory {
 public:
  struct Meta {
    std::string type;
    size_t allocated{0};
    size_t capacity{0};
  };

  static void remove(const std::string& name);

  virtual ~Memory() {}

  virtual bool init() = 0;
  virtual bool dump() = 0;
  virtual bool reset() = 0;

  virtual bool readOnly() const = 0;

  virtual int64_t allocate(size_t size) = 0;
  virtual size_t getAllocatedSize() const = 0;

  virtual void* address(int64_t offset) const = 0;
};

}  // namespace crystal
