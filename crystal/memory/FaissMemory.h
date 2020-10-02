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
#include <memory>
#include <string>
#include <fcntl.h>

#include "crystal/memory/Memory.h"

namespace faiss {
struct Index;
} // namespace faiss

namespace crystal {

class FaissMemory : public Memory {
 public:
  FaissMemory(const char* name, int flags, const void* extra);
  virtual ~FaissMemory();

  bool init() override;
  bool dump() override;
  bool reset() override;

  bool readOnly() const override;

  int64_t allocate(size_t size) override;
  size_t getAllocatedSize() const override;

  void* address(int64_t offset) const override;

 private:
  bool load();

  std::string name_;
  int flags_;
  faiss::Index* base_{nullptr};
  std::string desc_;
  int64_t dimension_;
  int metric_;
  Meta meta_;
};

//////////////////////////////////////////////////////////////////////

inline bool FaissMemory::readOnly() const {
  return flags_ == O_RDONLY;
}

inline void* FaissMemory::address(int64_t offset) const {
  assert(offset == kMemStart);
  return base_;
}

}  // namespace crystal
