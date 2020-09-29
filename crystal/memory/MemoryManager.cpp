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

#include "crystal/memory/MemoryManager.h"

#include "crystal/foundation/Logging.h"
#include "crystal/foundation/String.h"
#include "crystal/memory/MMapMemory.h"

namespace crystal {

#define CRYSTAL_MEMORY_TYPE_STR(type) #type

namespace {

static const char* sMemoryTypeStrings[] = {
  CRYSTAL_MEMORY_TYPE_GEN(CRYSTAL_MEMORY_TYPE_STR)
};

}

const char* memoryTypeToString(int type) {
  return sMemoryTypeStrings[type];
}

static std::string toMemPath(const std::string& path, int type) {
  std::string typeStr = memoryTypeToString(type);
  toLower(typeStr);
  return path + "." + typeStr.substr(3);
}

void MemoryManager::remove(const std::string& path) {
  for (int i = 0; i < kMemMax; ++i) {
    Memory::remove(toMemPath(path, i));
  }
}

void MemoryManager::dump() {
  for (int i = 0; i < kMemMax; ++i) {
    if (memArray_[i]) {
      if (!memArray_[i]->dump()) {
        auto path = toMemPath(path_, i);
        CRYSTAL_LOG(ERROR) << "dump memory '" << path << "' failed";
      }
    }
  }
}

void MemoryManager::createMemory(int type) {
  auto path = toMemPath(path_, type);
  int flags = readOnly_ ? O_RDONLY : O_RDWR | O_CREAT;
  std::unique_ptr<Memory> mem;
  switch (type) {
    case kMemSimple:
    case kMemRecyc:
    case kMemHash:
    case kMemBit:
      mem = std::make_unique<MMapMemory>(path.c_str(), flags);
      break;
    default:
      return;
  }
  if (!mem->init()) {
    CRYSTAL_LOG(ERROR) << "init memory '" << path << "' failed";
  }
  memArray_[type] = std::move(mem);
}

} // namespace crystal
