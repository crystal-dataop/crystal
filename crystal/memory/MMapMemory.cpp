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

#include "crystal/memory/MMapMemory.h"

#include <filesystem>
#include <unistd.h>

#include "crystal/foundation/Exception.h"
#include "crystal/foundation/File.h"
#include "crystal/foundation/Logging.h"
#include "crystal/foundation/json.h"

namespace crystal {

MMapMemory::MMapMemory(const char* name,
                       int flags,
                       size_t expandSize,
                       size_t maxSize)
    : name_(name),
      flags_(flags),
      expandSize_(expandSize),
      maxSize_(maxSize),
      data_(name, flags) {
  meta_.type = "crystal_mmap";
}

bool MMapMemory::init() {
  if (std::filesystem::exists(name_ + ".meta")) {
    if (!load()) {
      return false;;
    }
  }
  int prot = PROT_READ;
  if (flags_ != O_RDONLY) {
    prot |= PROT_WRITE;
  }
  if (!data_.init(maxSize_, prot)) {
    return false;
  }
  base_ = reinterpret_cast<uint8_t*>(data_.get());
  return true;
}

bool MMapMemory::dump() {
  if (flags_ == O_RDONLY) {
    return false;
  }
  data_.sync(meta_.allocated);
  meta_.capacity = meta_.allocated;
  return writeFile(
      toCson(
          dynamic::object
            ("type", meta_.type)
            ("allocated", meta_.allocated)
            ("capacity", meta_.capacity)),
      (name_ + ".meta").c_str());
}

bool MMapMemory::reset() {
  if (flags_ == O_RDONLY) {
    CRYSTAL_THROW(RuntimeError, "reset on read-only mmap");
  }
  meta_.allocated = 0;
  return true;
}

int64_t MMapMemory::allocate(size_t size) {
  if (flags_ == O_RDONLY) {
    CRYSTAL_THROW(RuntimeError, "allocate on read-only mmap");
  }
  if (size == 0) {
    CRYSTAL_LOG(ERROR) << "allocate with size=0";
    return 0;
  }
  if (meta_.allocated + size > meta_.capacity) {
    if (!expand(size)) {
      CRYSTAL_LOG(ERROR) << "allocate expand failed";
      return 0;
    }
  }
  int64_t pos = meta_.allocated + kMemStart;
  meta_.allocated += size;
  return pos;
}

bool MMapMemory::load() {
  std::string meta;
  if (!readFile((name_ + ".meta").c_str(), meta)) {
    return false;
  }
  auto j = parseCson(meta);
  if (meta_.type != j["type"].asString()) {
    return false;
  }
  meta_.allocated = j["allocated"].asInt();
  meta_.capacity = j["capacity"].asInt();

  size_t fileSize = data_.file().size();
  if (fileSize > maxSize_ ||
      fileSize < meta_.capacity ||
      meta_.capacity < meta_.allocated) {
    return false;
  }
  return true;
}

static bool flush(int fd, size_t n) {
  char c = 0;
  return pwrite(fd, &c, 1, n - 1) == 1;
}

bool MMapMemory::expand(size_t size) {
  if (meta_.capacity + size > maxSize_) {
    CRYSTAL_LOG(ERROR) << "expand exceed max size: "
      << meta_.capacity << "+" << size << ">" << maxSize_;
    return false;
  }
  size = std::max(size, expandSize_);
  size = std::min(size, maxSize_ - meta_.capacity);
  if (!flush(data_.file().fd(), meta_.capacity + size)) {
    CRYSTAL_LOG(ERROR) << "expand flush failed";
    return false;
  }
  meta_.capacity += size;
  return true;
}

}  // namespace crystal
