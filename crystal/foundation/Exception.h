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

#include <cerrno>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include "crystal/foundation/Conv.h"
#include "crystal/foundation/Likely.h"

namespace crystal {

std::string recordBacktrace();
std::string recordBacktrace(const std::string& msg);

template <class Ex>
class ExceptionBase : public Ex {
 public:
  template <class... Args>
  explicit ExceptionBase(Args&&... args)
      : Ex(recordBacktrace(toString(std::forward<Args>(args)...))) {
  }
};

// logic error
typedef ExceptionBase<std::logic_error> LogicError;
typedef ExceptionBase<std::invalid_argument> InvalidArgument;
typedef ExceptionBase<std::domain_error> DomainError;
typedef ExceptionBase<std::length_error> LengthError;
typedef ExceptionBase<std::out_of_range> OutOfRange;

// runtime error
typedef ExceptionBase<std::runtime_error> RuntimeError;
typedef ExceptionBase<std::range_error> RangeError;
typedef ExceptionBase<std::overflow_error> OverflowError;
typedef ExceptionBase<std::underflow_error> UnderflowError;
typedef ExceptionBase<std::runtime_error> NotImplementedError;

class SystemError : public std::system_error {
 public:
  template <class... Args>
  explicit SystemError(std::error_code ec, Args&&... args)
      : std::system_error(
          ec, recordBacktrace(toString(std::forward<Args>(args)...))) {
  }
  template <class... Args>
  explicit SystemError(int ev, const std::error_category& c, Args&&... args)
      : std::system_error(
          ev, c, recordBacktrace(toString(std::forward<Args>(args)...))) {
  }
};

#define CRYSTAL_CODE_POS \
  "@(", __FILE__, ":", __LINE__, ":'", __PRETTY_FUNCTION__, "') "

#define CRYSTAL_THROW(E, ...) \
  throw E(#E, CRYSTAL_CODE_POS, ##__VA_ARGS__)

#define CRYSTAL_CHECK_THROW(cond, E, ...)                               \
  do {                                                                  \
    if (!(cond)) {                                                      \
      throw E("Check failed: " #cond, CRYSTAL_CODE_POS, ##__VA_ARGS__); \
    }                                                                   \
  } while (0)

template <class... Args>
[[noreturn]] void throwSystemError(Args&&... args) {
  throw SystemError(errno, std::system_category(), std::forward<Args>(args)...);
}

template <class... Args>
void checkUnixError(ssize_t ret, Args&&... args) {
  if (CRYSTAL_UNLIKELY(ret == -1)) {
    throwSystemError(std::forward<Args>(args)...);
  }
}

template <class... Args>
void checkFopenError(FILE* fp, Args&&... args) {
  if (CRYSTAL_UNLIKELY(!fp)) {
    throwSystemError(std::forward<Args>(args)...);
  }
}

} // namespace crystal
