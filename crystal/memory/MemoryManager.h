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
#include <string>

#include "crystal/memory/Memory.h"

namespace crystal {

#define CRYSTAL_MEMORY_TYPE_GEN(x)  \
  x(MemSimple),                     \
  x(MemRecyc),                      \
  x(MemHash),                       \
  x(MemBit),                        \
  x(MemMax)

#define CRYSTAL_MEMORY_TYPE_ENUM(type) k##type

enum MemoryType {
  CRYSTAL_MEMORY_TYPE_GEN(CRYSTAL_MEMORY_TYPE_ENUM)
};

#undef CRYSTAL_MEMORY_TYPE_ENUM

const char* memoryTypeToString(int type);

class MemoryManager {
 public:
  static void remove(const std::string& path);

  MemoryManager(const char* path, bool readOnly)
      : path_(path), readOnly_(readOnly) {}

  Memory* getMemory(int type);

  void dump();

 private:
  void createMemory(int type);

  std::string path_;
  bool readOnly_;
  std::unique_ptr<Memory> memArray_[kMemMax];
};

//////////////////////////////////////////////////////////////////////

inline Memory* MemoryManager::getMemory(int type) {
  if (type < 0 || type >= kMemMax) {
    return nullptr;
  }
  if (!memArray_[type]) {
    createMemory(type);
  }
  return memArray_[type].get();
}

} // namespace crystal
