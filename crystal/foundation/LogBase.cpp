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

#include "crystal/foundation/LogBase.h"

#include <filesystem>
#include <iomanip>
#include <iterator>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "crystal/foundation/Conv.h"
#include "crystal/foundation/File.h"
#include "crystal/foundation/ThreadId.h"
#include "crystal/foundation/ThreadName.h"

namespace fs = std::filesystem;

namespace {

size_t getSize(int fd) {
  if (fd >= 0) {
    struct stat fs;
    if (fstat(fd, &fs) != -1) {
      return fs.st_size;
    }
  }
  return 0;
}

} // namespace

namespace crystal {
namespace logging {

namespace detail {

inline char getLevelLabel(int level) {
  return level >= 0 ? "DIWEF"[level] : 'V';
}

size_t writeLogHeader(bool istty,
                      char* buffer,
                      size_t size,
                      int level,
                      const char* file,
                      int line,
                      const char* traceid) {
  static std::string sTTYColors[] = {
    "\x1b[34m",
    "\x1b[37m",
    "\x1b[33m",
    "\x1b[1;31m",
    "\x1b[1;31m",
    "\x1b[35m",
  };

  char* p = buffer;
  size_t n = size;
  ssize_t r;

  if (istty && level != LOG_INFO) {
    auto& c = sTTYColors[level >= 0 ? level : std::size(sTTYColors) - 1];
    memcpy(p, c.data(), c.size());
    p += c.size();
    n -= c.size();
  }

  *p++ = '[';
  *p++ = getLevelLabel(level);
  n -= 2;

  uint64_t now = timestampNow();
  time_t t = now / 1000000;
  struct tm tm;
  ::localtime_r(&t, &tm);
  r = strftime(p, n, " %y%m%d %T", &tm);
  p += r;
  n -= r;

  int tid = getOSThreadID();
  r = snprintf(p, n, ".%06llu %5d %s:%d] ", now % 1000000, tid, file, line);
  p += r;
  n -= r;

  if (istty && level != LOG_INFO) {
    static std::string sTTYColorEnd = "\x1b[0m";
    memcpy(p, sTTYColorEnd.data(), sTTYColorEnd.size());
    p += sTTYColorEnd.size();
    n -= sTTYColorEnd.size();
  }
  if (traceid) {
    r = snprintf(p, n, "trace(%s) ", traceid);
    p += r;
    n -= r;
  }
  return size - n;
}

} // namespace detail

BaseLogger::BaseLogger(const std::string& name)
  : name_(name),
    fd_(-1),
    level_(1),
    rotate_(0),
    splitSize_(0),
    async_(false) {
  std::thread handle = std::thread(&BaseLogger::run, this);
  setThreadName(handle.get_id(), "LogThread");
  handle.detach();
}

BaseLogger::~BaseLogger() {
  close();
}

void BaseLogger::run() {
  while (true) {
    queue_.sweep([&](std::string&& message) {
      write(std::move(message));
    });
    usleep(1000);
  }
}

void BaseLogger::log(std::string&& message, bool async) {
  if (async_ && async) {
    queue_.insertHead(std::move(message));
  } else {
    write(std::move(message));
  }
}

void BaseLogger::setLogFile(const std::string& file) {
  if (!file.empty() && file != file_) {
    file_ = file;

    std::lock_guard<std::mutex> guard(lock_);
    close();
    open();
  }
}

void BaseLogger::setLevel(int level) {
  level_ = level;
}

void BaseLogger::setRotate(int rotate, size_t size) {
  rotate_ = rotate;
  splitSize_ = size;
}

void BaseLogger::setAsync(bool async) {
  async_ = async;
}

void BaseLogger::setOptions(const Options& opts) {
  setLogFile(opts.logFile);
  setLevel(opts.level);
  setRotate(opts.rotate, opts.splitSize);
  setAsync(opts.async);
}

bool BaseLogger::isTTY() const {
  return fd_ >= 0 ? isatty(fd_) : true;
}

void BaseLogger::open() {
  // Failed to -1, will use stderr.
  fd_ = ::open(file_.c_str(), O_RDWR | O_APPEND | O_CREAT, 0666);
}

void BaseLogger::close() {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
}

void BaseLogger::split() {
  fs::path file(file_);
  for (auto& p : fs::directory_iterator(file.parent_path())) {
    auto path = p.path();
    if (path.stem() == file.filename()) {
      int no = atoi(path.extension().c_str());
      if (rotate_ > 0 && no >= rotate_) {
        fs::remove(path);
      } else if (no > 0) {
        fs::rename(path, path.replace_extension(to<std::string>(no+1)));
      }
    }
  }

  std::lock_guard<std::mutex> guard(lock_);
  close();
  fs::rename(file, file.replace_extension("1"));
  open();
}

void BaseLogger::write(std::string&& message) {
  int fd;
  {
    std::lock_guard<std::mutex> guard(lock_);
    fd = fd_ >= 0 ? fd_ : STDERR_FILENO;
    writeFull(fd, message.c_str(), message.size());
  }
  if (splitSize_ > 0 && getSize(fd) >= splitSize_) {
    split();
  }
}

LogMessage::LogMessage(
    BaseLogger* logger,
    int level,
    const char* file,
    int line,
    const std::string& traceid)
  : out_(buf_, kBufSize)
  , logger_(logger)
  , level_(level)
  , errno_(errno) {
  out_.advance(
      detail::writeLogHeader(
          logger_->isTTY(),
          buf_, kBufSize, level, file, line,
          traceid.empty() ? nullptr : traceid.c_str()));
}

LogMessage::~LogMessage() {
  out_ << std::endl;
  logger_->log(out_.str(), level_ < LOG_ERROR);
  errno = errno_;
  if (level_ == LOG_FATAL) {
    abort();
  }
}

RawLogMessage::RawLogMessage(BaseLogger* logger)
  : out_(buf_, sizeof(buf_))
  , logger_(logger)
  , errno_(errno) {
}

RawLogMessage::~RawLogMessage() {
  out_ << std::endl;
  logger_->log(out_.str());
  errno = errno_;
}

} // namespace logging
} // namespace crystal
