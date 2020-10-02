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

#include "crystal/memory/FaissMemory.h"

#include <filesystem>

#include <faiss/Index.h>
#include <faiss/index_factory.h>
#include <faiss/index_io.h>

#include "crystal/foundation/Exception.h"
#include "crystal/foundation/File.h"
#include "crystal/foundation/json.h"
#include "crystal/storage/index/vector/VectorMeta.h"

namespace crystal {

FaissMemory::FaissMemory(const char* name, int flags, const void* extra)
    : name_(name), flags_(flags) {
  auto vm = reinterpret_cast<const VectorMeta*>(extra);
  desc_ = vm->desc;
  dimension_ = vm->dimension;
  metric_ = vm->metric;
  meta_.type = "crystal_faiss";
}

FaissMemory::~FaissMemory() {
  if (base_) {
    delete base_;
  }
}

bool FaissMemory::init() {
  if (std::filesystem::exists(name_ + ".meta")) {
    if (!load()) {
      return false;
    }
  } else {
    base_ = faiss::index_factory(dimension_,
                                 desc_.c_str(),
                                 (faiss::MetricType)metric_);
  }
  return true;
}

bool FaissMemory::dump() {
  if (flags_ == O_RDONLY) {
    return false;
  }
  faiss::write_index(base_, name_.c_str());
  return writeFile(
      toCson(
          dynamic::object
            ("type", meta_.type)),
      (name_ + ".meta").c_str());
}

bool FaissMemory::reset() {
  if (flags_ == O_RDONLY) {
    CRYSTAL_THROW(RuntimeError, "reset on read-only mmap");
  }
  return true;
}

int64_t FaissMemory::allocate(size_t size) {
  CRYSTAL_THROW(RuntimeError, "unsupport on faiss");
}

size_t FaissMemory::getAllocatedSize() const {
  CRYSTAL_THROW(RuntimeError, "unsupport on faiss");
}

bool FaissMemory::load() {
  std::string meta;
  if (!readFile((name_ + ".meta").c_str(), meta)) {
    return false;
  }
  auto j = parseCson(meta);
  if (meta_.type != j["type"].asString()) {
    return false;
  }
  base_ = faiss::read_index(name_.c_str());
  return true;
}

}  // namespace crystal
