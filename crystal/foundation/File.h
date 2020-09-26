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

#include <algorithm>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "crystal/foundation/ScopeGuard.h"

namespace crystal {

/**
 * A File represents an open file. Modified from folly.
 */
class File {
 public:
  File() noexcept;

  explicit File(int fd, bool ownsFd = false) noexcept;
  explicit File(const char* name, int flags = O_RDONLY, mode_t mode = 0666);
  explicit File(
      const std::string& name, int flags = O_RDONLY, mode_t mode = 0666);

  ~File();

  static File temporary();

  int fd() const { return fd_; }

  explicit operator bool() const {
    return fd_ != -1;
  }

  size_t size() const;

  File dup() const;

  void close();

  bool closeNoThrow();

  int release() noexcept;

  void swap(File& other);

  File(File&&) noexcept;
  File& operator=(File&&);

 private:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  int fd_;
  bool ownsFd_;
};

/**
 * Provides random access to a file.
 * This implementation currently works by mmaping the file and working
 * from the in-memory copy.
 */
class FileContents {
 public:
  explicit FileContents(const File& file);

  ~FileContents();

  size_t getFileLength() { return fileLen_; }

  void copy(size_t offset, void* buf, size_t length);

  size_t copyPartial(size_t offset, void* buf, size_t maxLength);

  template <typename T = void>
  const T* get(size_t offset, size_t length) {
    return reinterpret_cast<const T*>(getHelper(offset, length));
  }

 private:
  FileContents(const FileContents&) = delete;
  FileContents& operator=(const FileContents&) = delete;

  const void* getHelper(size_t offset, size_t length);

  File file_;
  size_t fileLen_;
  const void* map_;
};

ssize_t readFull(int fd, void* buf, size_t n);
ssize_t writeFull(int fd, const void* buf, size_t n);

/**
 * Read entire file (if num_bytes is defaulted) or no more than
 * num_bytes (otherwise) into container *out.
 */
template <class Container>
bool readFile(
    int fd,
    Container& out,
    size_t num_bytes = std::numeric_limits<size_t>::max()) {
  static_assert(sizeof(out[0]) == 1, "require byte-sized elements");
  struct stat buf;
  if (fstat(fd, &buf) == -1) {
    return false;
  }
  out.resize(std::min(size_t(buf.st_size), num_bytes));
  return out.empty() ||
    readFull(fd, &out[0], out.size()) == static_cast<ssize_t>(out.size());
}

template <class Container>
bool readFile(
    const char* file_name,
    Container& out,
    size_t num_bytes = std::numeric_limits<size_t>::max()) {
  int fd = open(file_name, O_RDONLY, 0666);
  if (fd == -1) {
    return false;
  }

  CRYSTAL_SCOPE_EXIT {
    close(fd);
  };
  return readFile(fd, out, num_bytes);
}

/**
 * Writes container to file.
 */
template <class Container>
bool writeFile(const Container& data,
               const char* filename,
               int flags = O_WRONLY | O_CREAT | O_TRUNC,
               mode_t mode = 0666) {
  static_assert(sizeof(data[0]) == 1, "require byte-sized elements");
  int fd = open(filename, flags, mode);
  if (fd == -1) {
    return false;
  }

  CRYSTAL_SCOPE_EXIT {
    close(fd);
  };
  return data.empty() ||
    writeFull(fd, &data[0], data.size()) == static_cast<ssize_t>(data.size());
}

}  // namespace crystal
