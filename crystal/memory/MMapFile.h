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

#include <sys/mman.h>

#include "crystal/foundation/File.h"

namespace crystal {

class MMapFile {
 public:
  MMapFile(const char* name, int flags)
      : file_(name, flags) {}

  MMapFile(File&& file)
      : file_(std::move(file)) {}

  ~MMapFile();

  bool init(size_t size, int prot);

  bool sync(size_t size = -1);

  const File& file() const;

  void* get() const;

  size_t size() const;

 private:
  File file_;
  size_t size_{0};
  void* mapping_{nullptr};
};

//////////////////////////////////////////////////////////////////////

inline const File& MMapFile::file() const {
  return file_;
}

inline void* MMapFile::get() const {
  return mapping_;
}

inline size_t MMapFile::size() const {
  return size_;
}

}  // namespace crystal
