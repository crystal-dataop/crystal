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

#include "crystal/memory/MMapFile.h"

#include <unistd.h>

namespace crystal {

MMapFile::~MMapFile() {
  if (mapping_) {
    munmap(mapping_, size_);
  }
}

bool MMapFile::init(size_t size, int prot) {
  size_ = size;
  mapping_ = mmap(nullptr, size_, prot, MAP_SHARED, file_.fd(), 0);
  if (mapping_ == MAP_FAILED) {
    return false;
  }
  return true;
}

bool MMapFile::sync(size_t size) {
  size = size != size_t(-1) ? size : size_;
  if (size > 0 && msync(mapping_, size, MS_SYNC) == -1) {
    return false;
  }
  if (ftruncate(file_.fd(), size) == -1) {
    return false;
  }
  return true;
}

}  // namespace crystal
