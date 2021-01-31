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

#include "crystal/foundation/String.h"

#include <array>
#include <cctype>
#include <cstdarg>
#include <cstdio>

#include "crystal/crystal-config.h"
#include "crystal/foundation/Array.h"
#include "crystal/foundation/Conv.h"
#include "crystal/foundation/Logging.h"
#include "crystal/foundation/ScopeGuard.h"

namespace crystal {

namespace detail {

struct string_table_hex_make_item {
  constexpr unsigned char operator()(std::size_t index) const {
    // clang-format off
    return
        index >= '0' && index <= '9' ? index - '0' :
        index >= 'a' && index <= 'f' ? index - 'a' + 10 :
        index >= 'A' && index <= 'F' ? index - 'A' + 10 :
        16;
    // clang-format on
  }
};

constexpr decltype(hexTable) hexTable =
    make_array_with<256>(string_table_hex_make_item{});

} // namespace detail

static inline bool is_space(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

std::string_view ltrimWhitespace(std::string_view sv) {
  while (!sv.empty() && is_space(sv.front())) {
    sv.remove_prefix(1);
  }
  return sv;
}

std::string_view rtrimWhitespace(std::string_view sv) {
  while (!sv.empty() && is_space(sv.back())) {
    sv.remove_suffix(1);
  }
  return sv;
}

namespace {

int stringAppendfImplHelper(char* buf,
                            size_t bufsize,
                            const char* format,
                            va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  int bytes_used = vsnprintf(buf, bufsize, format, args_copy);
  va_end(args_copy);
  return bytes_used;
}

void stringAppendfImpl(std::string& output, const char* format, va_list args) {
  std::array<char, 128> inline_buffer;

  int bytes_used = stringAppendfImplHelper(
      inline_buffer.data(), inline_buffer.size(), format, args);
  if (bytes_used < 0) {
    throw std::runtime_error(toString(
        "Invalid format string; snprintf returned negative "
        "with format string: ",
        format));
  }

  if (static_cast<size_t>(bytes_used) < inline_buffer.size()) {
    output.append(inline_buffer.data(), size_t(bytes_used));
    return;
  }

  std::unique_ptr<char[]> heap_buffer(new char[size_t(bytes_used + 1)]);
  int final_bytes_used = stringAppendfImplHelper(
      heap_buffer.get(), size_t(bytes_used + 1), format, args);
  // The second call can take fewer bytes if, for example, we were printing a
  // string buffer with null-terminating char using a width specifier -
  // vsnprintf("%.*s", buf.size(), buf)
  CRYSTAL_CHECK(bytes_used >= final_bytes_used);

  output.append(heap_buffer.get(), size_t(final_bytes_used));
}

} // namespace

std::string stringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  CRYSTAL_SCOPE_EXIT {
    va_end(ap);
  };
  return stringVPrintf(format, ap);
}

std::string stringVPrintf(const char* format, va_list ap) {
  std::string ret;
  stringAppendfImpl(ret, format, ap);
  return ret;
}

std::string errnoStr(int err) {
  int savedErrno = errno;

  char buf[1024];
  buf[0] = '\0';

  std::string result;

#if defined(CRYSTAL_HAVE_XSI_STRERROR_R) || defined(__APPLE__)
  // Using XSI-compatible strerror_r
  int r = strerror_r(err, buf, sizeof(buf));
  if (r != 0) {
    result = toString(
      "Unknown error ", err,
      " (strerror_r failed with error ", errno, ")");
  } else {
    result.assign(buf);
  }
#else
  // Using GNU strerror_r
  result.assign(strerror_r(err, buf, sizeof(buf)));
#endif

  errno = savedErrno;

  return result;
}

void toLower(char* str, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    std::tolower(str[i]);
  }
}

} // namespace crystal
