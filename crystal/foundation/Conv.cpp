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

#include "crystal/foundation/Conv.h"

namespace crystal {
namespace detail {

namespace {

// Check if ASCII is really ASCII
using IsAscii =
    std::bool_constant<'A' == 65 && 'Z' == 90 && 'a' == 97 && 'z' == 122>;

// The code in this file that uses tolower() really only cares about
// 7-bit ASCII characters, so we can take a nice shortcut here.
inline char tolower_ascii(char in) {
  return IsAscii::value ? in | 0x20 : char(std::tolower(in));
}

inline bool bool_str_cmp(const char** b, size_t len, const char* value) {
  // Can't use strncasecmp, since we want to ensure that the full value matches
  const char* p = *b;
  const char* e = *b + len;
  const char* v = value;
  while (*v != '\0') {
    if (p == e || tolower_ascii(*p) != *v) { // value is already lowercase
      return false;
    }
    ++p;
    ++v;
  }

  *b = p;
  return true;
}

} // namespace anon

bool str_to_bool(std::string_view* src) {
  auto b = src->begin(), e = src->end();
  for (;; ++b) {
    if (b >= e) {
      throw std::range_error("empty input string");
    }
    if (!std::isspace(*b)) {
      break;
    }
  }

  bool result;
  size_t len = size_t(e - b);
  switch (*b) {
    case '0':
    case '1': {
      result = false;
      for (; b < e && isdigit(*b); ++b) {
        if (result || (*b != '0' && *b != '1')) {
          throw std::overflow_error("bool overflow");
        }
        result = (*b == '1');
      }
      break;
    }
    case 'y':
    case 'Y':
      result = true;
      if (!bool_str_cmp(&b, len, "yes")) {
        ++b;  // accept the single 'y' character
      }
      break;
    case 'n':
    case 'N':
      result = false;
      if (!bool_str_cmp(&b, len, "no")) {
        ++b;
      }
      break;
    case 't':
    case 'T':
      result = true;
      if (!bool_str_cmp(&b, len, "true")) {
        ++b;
      }
      break;
    case 'f':
    case 'F':
      result = false;
      if (!bool_str_cmp(&b, len, "false")) {
        ++b;
      }
      break;
    case 'o':
    case 'O':
      if (bool_str_cmp(&b, len, "on")) {
        result = true;
      } else if (bool_str_cmp(&b, len, "off")) {
        result = false;
      } else {
        throw std::range_error("invalid value");
      }
      break;
    default:
      throw std::range_error("invalid value");
  }

  src->remove_prefix(b - src->begin());

  return result;
}

/**
 * std::string_view to double, with progress information. Alters the
 * std::string_view parameter to munch the already-parsed characters.
 */
template <class Tgt>
Tgt str_to_floating(std::string_view* src) {
  using namespace double_conversion;
  static StringToDoubleConverter
    conv(StringToDoubleConverter::ALLOW_TRAILING_JUNK
         | StringToDoubleConverter::ALLOW_LEADING_SPACES,
         0.0,
         // return this for junk input string
         std::numeric_limits<double>::quiet_NaN(),
         nullptr, nullptr);

  if (src->empty()) {
    throw std::range_error("empty input string");
  }

  int length;
  auto result = conv.StringToDouble(src->data(),
                                    static_cast<int>(src->size()),
                                    &length); // processed char count

  if (!std::isnan(result)) {
    // If we get here with length = 0, the input string is empty.
    // If we get here with result = 0.0, it's either because the string
    // contained only whitespace, or because we had an actual zero value
    // (with potential trailing junk). If it was only whitespace, we
    // want to raise an error; length will point past the last character
    // that was processed, so we need to check if that character was
    // whitespace or not.
    if (length == 0 ||
        (result == 0.0 && std::isspace((*src)[size_t(length) - 1]))) {
      throw std::range_error("empty input string");
    }
    if (length >= 2) {
      const char* suffix = src->data() + length - 1;
      // double_conversion doesn't update length correctly when there is an
      // incomplete exponent specifier. Converting "12e-f-g" shouldn't consume
      // any more than "12", but it will consume "12e-".

      // "123-" should only parse "123"
      if (*suffix == '-' || *suffix == '+') {
        --suffix;
        --length;
      }
      // "12e-f-g" or "12euro" should only parse "12"
      if (*suffix == 'e' || *suffix == 'E') {
        --length;
      }
    }
    src->remove_prefix(size_t(length));
    return Tgt(result);
  }

  auto* e = src->end();
  auto* b =
      std::find_if_not(src->begin(), e, [](char c) { return std::isspace(c); });

  // There must be non-whitespace, otherwise we would have caught this above
  assert(b < e);
  size_t size = size_t(e - b);

  bool negative = false;
  if (*b == '-') {
    negative = true;
    ++b;
    --size;
  }

  result = 0.0;

  switch (tolower_ascii(*b)) {
    case 'i':
      if (size >= 3 && tolower_ascii(b[1]) == 'n' &&
          tolower_ascii(b[2]) == 'f') {
        if (size >= 8 && tolower_ascii(b[3]) == 'i' &&
            tolower_ascii(b[4]) == 'n' && tolower_ascii(b[5]) == 'i' &&
            tolower_ascii(b[6]) == 't' && tolower_ascii(b[7]) == 'y') {
          b += 8;
        } else {
          b += 3;
        }
        result = std::numeric_limits<Tgt>::infinity();
      }
      break;

    case 'n':
      if (size >= 3 && tolower_ascii(b[1]) == 'a' &&
          tolower_ascii(b[2]) == 'n') {
        b += 3;
        result = std::numeric_limits<Tgt>::quiet_NaN();
      }
      break;

    default:
      break;
  }

  if (result == 0.0) {
    // All bets are off
    throw std::range_error("string to float error");
  }

  if (negative) {
    result = -result;
  }

  src->remove_prefix(b - src->begin());

  return Tgt(result);
}

template float str_to_floating<float>(std::string_view* src);
template double str_to_floating<double>(std::string_view* src);

} // namespace detail
} // namespace crystal
