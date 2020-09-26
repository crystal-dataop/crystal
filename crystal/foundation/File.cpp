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

#include "crystal/foundation/File.h"

#include <unistd.h>
#include <sys/mman.h>

#include "crystal/foundation/Exception.h"
#include "crystal/foundation/Logging.h"

namespace crystal {

File::File() noexcept : fd_(-1), ownsFd_(false) {}

File::File(int fd, bool ownsFd) noexcept : fd_(fd), ownsFd_(ownsFd) {
  CRYSTAL_CHECK_GE(fd, -1) << "fd must be -1 or non-negative";
  CRYSTAL_CHECK(fd != -1 || !ownsFd) << "cannot own -1";
}

File::File(const char* name, int flags, mode_t mode)
  : fd_(::open(name, flags, mode))
  , ownsFd_(false) {
  if (fd_ == -1) {
    throwSystemError("open(", name, ") failed");
  }
  ownsFd_ = true;
}

File::File(const std::string& name, int flags, mode_t mode)
  : File(name.c_str(), flags, mode) {}

File::File(File&& other) noexcept
  : fd_(other.fd_)
  , ownsFd_(other.ownsFd_) {
  other.release();
}

File& File::operator=(File&& other) {
  closeNoThrow();
  swap(other);
  return *this;
}

File::~File() {
  auto fd = fd_;
  if (!closeNoThrow()) {  // ignore most errors
    DCHECK_NE(errno, EBADF) << "closing fd " << fd << ", it may already "
      << "have been closed. Another time, this might close the wrong FD.";
  }
}

/* static */ File File::temporary() {
  // make a temp file with tmpfile(), dup the fd, then return it in a File.
  FILE* tmpFile = tmpfile();
  checkFopenError(tmpFile, "tmpfile() failed");
  CRYSTAL_SCOPE_EXIT { fclose(tmpFile); };

  int fd = ::dup(fileno(tmpFile));
  checkUnixError(fd, "dup() failed");

  return File(fd, true);
}

int File::release() noexcept {
  int released = fd_;
  fd_ = -1;
  ownsFd_ = false;
  return released;
}

void File::swap(File& other) {
  using std::swap;
  swap(fd_, other.fd_);
  swap(ownsFd_, other.ownsFd_);
}

void swap(File& a, File& b) {
  a.swap(b);
}

size_t File::size() const {
  struct stat stat;
  checkUnixError(::fstat(fd_, &stat), "fstat() failed");
  return size_t(stat.st_size);
}

File File::dup() const {
  if (fd_ != -1) {
    int fd = ::dup(fd_);
    checkUnixError(fd, "dup() failed");

    return File(fd, true);
  }

  return File();
}

void File::close() {
  if (!closeNoThrow()) {
    throwSystemError("close() failed");
  }
}

bool File::closeNoThrow() {
  int r = ownsFd_ ? ::close(fd_) : 0;
  release();
  return r == 0;
}

FileContents::FileContents(const File& origFile)
    : file_(origFile.dup()), fileLen_(file_.size()), map_(nullptr) {
  // len of 0 for empty files results in invalid argument
  if (fileLen_ > 0) {
    map_ = ::mmap(nullptr, fileLen_, PROT_READ, MAP_SHARED, file_.fd(), 0);
    if (map_ == MAP_FAILED) {
      throwSystemError("mmap() failed");
    }
  }
}

FileContents::~FileContents() {
  if (map_) {
    ::munmap(const_cast<void*>(map_), fileLen_);
  }
}

void FileContents::copy(size_t offset, void* buf, size_t length) {
  if (copyPartial(offset, buf, length) != length) {
    CRYSTAL_LOG(FATAL) << "File too short or corrupt";
  }
}

size_t FileContents::copyPartial(size_t offset, void* buf, size_t maxLength) {
  if (offset >= fileLen_) {
    return 0;
  }
  size_t length = std::min(fileLen_ - offset, maxLength);
  std::memcpy(buf, static_cast<const char*>(map_) + offset, length);
  return length;
}

const void* FileContents::getHelper(size_t offset, size_t length) {
  if (length != 0 && offset + length > fileLen_) {
    CRYSTAL_LOG(FATAL) << "File too short or corrupt";
  }
  return static_cast<const char*>(map_) + offset;
}

template <class F>
ssize_t wrapFull(F f, int fd, void* buf, size_t count) {
  char* b = static_cast<char*>(buf);
  ssize_t totalBytes = 0;
  ssize_t r;
  do {
    r = f(fd, b, count);
    if (r == -1) {
      if (errno == EINTR) {
        continue;
      }
      return r;
    }

    totalBytes += r;
    b += r;
    count -= r;
  } while (r != 0 && count);  // 0 means EOF

  return totalBytes;
}

ssize_t readFull(int fd, void* buf, size_t n) {
  return wrapFull(read, fd, buf, n);
}

ssize_t writeFull(int fd, const void* buf, size_t n) {
  return wrapFull(write, fd, const_cast<void*>(buf), n);
}

}  // namespace crystal
