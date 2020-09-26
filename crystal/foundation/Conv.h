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

/*
 * Modified from folly.
 */

#pragma once

#include <charconv>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <type_traits>

#include <double-conversion/double-conversion.h>

#include "crystal/foundation/Likely.h"
#include "crystal/foundation/Traits.h"

namespace crystal {

/*
 * T -> itself
 */

template <class Tgt, class Src>
typename std::enable_if<
    std::is_same<Tgt, typename std::decay<Src>::type>::value,
    Tgt>::type
to(Src&& value) {
  return std::forward<Src>(value);
}

/*
 * arithmetic -> bool
 */

template <class Tgt, class Src>
typename std::enable_if<
    std::is_arithmetic<Src>::value && !std::is_same<Tgt, Src>::value &&
        std::is_same<Tgt, bool>::value,
    Tgt>::type
to(const Src& value) {
  return value != Src();
}

/*
 * anything -> string
 */

inline void toAppend(std::string* result, char value) {
  *result += value;
}

template <class Src>
typename std::enable_if<std::is_convertible<Src, const char*>::value>::type
toAppend(std::string* result, Src value) {
  const char* c = value;
  if (c) {
    result->append(value);
  }
}

inline void toAppend(std::string* result, const std::string& value) {
  result->append(value);
}

inline void toAppend(std::string* result, std::string_view value) {
  result->append(value.data(), value.size());
}

inline void toAppend(std::string* result, bool value) {
  result->append(value ? "true" : "false");
}

template <class Src>
typename std::enable_if<
    std::is_integral<Src>::value && !std::is_same<Src, bool>::value>::type
toAppend(std::string* result, Src value) {
  char buffer[20];
  auto [p, ec] = std::to_chars(buffer, buffer + 20, value);
  result->append(buffer, p);
}

template <class Src>
typename std::enable_if<std::is_enum<Src>::value>::type
toAppend(std::string* result, Src value) {
  toAppend(
      static_cast<typename std::underlying_type<Src>::type>(value), result);
}

namespace detail {
constexpr int kConvMaxDecimalInShortestLow = -6;
constexpr int kConvMaxDecimalInShortestHigh = 21;
} // namespace detail

template <class Src>
typename std::enable_if<std::is_floating_point<Src>::value>::type
toAppend(
  std::string* result,
  Src value,
  double_conversion::DoubleToStringConverter::DtoaMode mode,
  unsigned int numDigits) {
  using namespace double_conversion;
  DoubleToStringConverter
    conv(DoubleToStringConverter::NO_FLAGS,
         "Infinity", "NaN", 'E',
         detail::kConvMaxDecimalInShortestLow,
         detail::kConvMaxDecimalInShortestHigh,
         6,   // max leading padding zeros
         1);  // max trailing padding zeros
  char buffer[256];
  StringBuilder builder(buffer, sizeof(buffer));
  switch (mode) {
    case DoubleToStringConverter::SHORTEST:
      conv.ToShortest(value, &builder);
      break;
    case DoubleToStringConverter::SHORTEST_SINGLE:
      conv.ToShortestSingle(static_cast<float>(value), &builder);
      break;
    case DoubleToStringConverter::FIXED:
      conv.ToFixed(value, int(numDigits), &builder);
      break;
    case DoubleToStringConverter::PRECISION:
      conv.ToPrecision(value, int(numDigits), &builder);
      break;
  }
  const size_t length = size_t(builder.position());
  builder.Finalize();
  result->append(buffer, length);
}

template <class Src>
typename std::enable_if<std::is_floating_point<Src>::value>::type
toAppend(std::string* result, Src value) {
  toAppend(
    result, value, double_conversion::DoubleToStringConverter::SHORTEST, 0);
}

template <class Tgt, class Src>
typename std::enable_if<
    std::is_same<Tgt, std::string>::value && !std::is_same<Tgt, Src>::value,
    Tgt>::type
to(const Src& value) {
  Tgt result;
  toAppend(&result, value);
  return result;
}

namespace detail {

/**
 * Bool to integral/float doesn't need any special checks, and this
 * overload means we aren't trying to see if a bool is less than
 * an integer.
 */
template <class Tgt>
typename std::enable_if<
    !std::is_same<Tgt, bool>::value &&
        (std::is_integral<Tgt>::value || std::is_floating_point<Tgt>::value),
    Tgt>::type
convertTo(const bool& value) {
  return static_cast<Tgt>(value ? 1 : 0);
}

/**
 * Checked conversion from integral to integral. The checks are only
 * performed when meaningful, e.g. conversion from int to long goes
 * unchecked.
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_integral<Src>::value && !std::is_same<Tgt, Src>::value &&
        !std::is_same<Tgt, bool>::value &&
        std::is_integral<Tgt>::value,
    Tgt>::type
convertTo(const Src& value) {
  /* static */ if (
      _t<std::make_unsigned<Tgt>>(std::numeric_limits<Tgt>::max()) <
      _t<std::make_unsigned<Src>>(std::numeric_limits<Src>::max())) {
    if (greater_than<Tgt, std::numeric_limits<Tgt>::max()>(value)) {
      throw std::overflow_error("positive overflow");
    }
  }
  /* static */ if (
      std::is_signed<Src>::value &&
      (!std::is_signed<Tgt>::value || sizeof(Src) > sizeof(Tgt))) {
    if (less_than<Tgt, std::numeric_limits<Tgt>::min()>(value)) {
      throw std::overflow_error("negative overflow");
    }
  }
  return static_cast<Tgt>(value);
}

/**
 * Checked conversion from floating to floating. The checks are only
 * performed when meaningful, e.g. conversion from float to double goes
 * unchecked.
 */
template <class Tgt, class Src>
typename std::enable_if<
    std::is_floating_point<Tgt>::value && std::is_floating_point<Src>::value &&
        !std::is_same<Tgt, Src>::value,
    Tgt>::type
convertTo(const Src& value) {
  /* static */ if (
      std::numeric_limits<Tgt>::max() < std::numeric_limits<Src>::max()) {
    if (value > std::numeric_limits<Tgt>::max()) {
      throw std::overflow_error("positive overflow");
    }
    if (value < std::numeric_limits<Tgt>::lowest()) {
      throw std::overflow_error("negative overflow");
    }
  }
  return static_cast<Tgt>(value);
}

/**
 * Check if a floating point value can safely be converted to an
 * integer value without triggering undefined behaviour.
 */
template <typename Tgt, typename Src>
inline typename std::enable_if<
    std::is_floating_point<Src>::value && std::is_integral<Tgt>::value &&
        !std::is_same<Tgt, bool>::value,
    bool>::type
checkConversion(const Src& value) {
  constexpr Src tgtMaxAsSrc = static_cast<Src>(std::numeric_limits<Tgt>::max());
  constexpr Src tgtMinAsSrc = static_cast<Src>(std::numeric_limits<Tgt>::min());
  if (value >= tgtMaxAsSrc) {
    if (value > tgtMaxAsSrc) {
      return false;
    }
    const Src mmax = std::nextafter(tgtMaxAsSrc, Src());
    if (static_cast<Tgt>(value - mmax) >
        std::numeric_limits<Tgt>::max() - static_cast<Tgt>(mmax)) {
      return false;
    }
  } else if (std::is_signed<Tgt>::value && value <= tgtMinAsSrc) {
    if (value < tgtMinAsSrc) {
      return false;
    }
    const Src mmin = std::nextafter(tgtMinAsSrc, Src());
    if (static_cast<Tgt>(value - mmin) <
        std::numeric_limits<Tgt>::min() - static_cast<Tgt>(mmin)) {
      return false;
    }
  }
  return true;
}

// Integers can always safely be converted to floating point values
template <typename Tgt, typename Src>
constexpr typename std::enable_if<
    std::is_integral<Src>::value && std::is_floating_point<Tgt>::value,
    bool>::type
checkConversion(const Src&) {
  return true;
}

// Also, floating point values can always be safely converted to bool
// Per the standard, any floating point value that is not zero will yield true
template <typename Tgt, typename Src>
constexpr typename std::enable_if<
    std::is_floating_point<Src>::value && std::is_same<Tgt, bool>::value,
    bool>::type
checkConversion(const Src&) {
  return true;
}

/**
 * Checked conversion from integral to floating point and back. The
 * result must be convertible back to the source type without loss of
 * precision. This seems Draconian but sometimes is what's needed, and
 * complements existing routines nicely. For various rounding
 * routines, see <math>.
 */
template <typename Tgt, typename Src>
typename std::enable_if<
    (std::is_integral<Src>::value && std::is_floating_point<Tgt>::value) ||
        (std::is_floating_point<Src>::value && std::is_integral<Tgt>::value),
    Tgt>::type
convertTo(const Src& value) {
  if (CRYSTAL_LIKELY(checkConversion<Tgt>(value))) {
    Tgt result = static_cast<Tgt>(value);
    if (CRYSTAL_LIKELY(checkConversion<Src>(result))) {
      Src witness = static_cast<Src>(result);
      if (CRYSTAL_LIKELY(value == witness)) {
        return result;
      }
    }
  }
  throw std::range_error("loss of precision");
}

/*
 * Conversions from string types to arithmetic types.
 */

bool str_to_bool(std::string_view* src);

template <typename T>
T str_to_floating(std::string_view* src);

extern template float str_to_floating<float>(std::string_view* src);
extern template double str_to_floating<double>(std::string_view* src);

template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, T>::type
convertTo(std::string_view* src) {
  return str_to_bool(src);
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
convertTo(std::string_view* src) {
  return str_to_floating<T>(src);
}

template <typename T>
typename std::enable_if<
    std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
convertTo(std::string_view* src) {
  T result;
  auto [p, ec] = std::from_chars(src->begin(), src->end(), result);
  if (ec == std::errc()) {
    return result;
  }
  throw std::range_error(std::make_error_code(ec).message());
}

template <class Tgt>
inline typename std::enable_if<std::is_arithmetic<Tgt>::value>::type
parseTo(std::string_view in, Tgt& out) {
  out = convertTo<Tgt>(&in);
}

template <class Tgt>
inline typename std::enable_if<std::is_enum<Tgt>::value>::type
parseTo(std::string_view in, Tgt& out) {
  typename std::underlying_type<Tgt>::type tmp{};
  parseTo(in, tmp);
  out = static_cast<Tgt>(tmp);
}

inline void parseTo(std::string_view in, std::string_view& out) {
  out = in;
}

inline void parseTo(std::string_view in, std::string& out) {
  out.clear();
  out.append(in.data(), in.size());
}

} // namespace detail

/*
 * integral/floating-point <-> integral/floating-point
 */

template <typename Tgt, typename Src>
typename std::enable_if<
    std::is_arithmetic<Src>::value && std::is_arithmetic<Tgt>::value &&
        !std::is_same<Tgt, Src>::value && !std::is_same<Tgt, bool>::value,
    Tgt>::type
to(const Src& value) {
  return detail::convertTo<Tgt>(value);
}

/*
 * string/string_view -> T
 */

template <class Tgt, class Src>
inline typename std::enable_if<
    !std::is_same<std::string_view, Tgt>::value,
    Tgt>::type
to(const std::string& src) {
  return to<Tgt>(std::string_view(src));
}

template <class Tgt>
inline typename std::enable_if<
    !std::is_same<std::string_view, Tgt>::value,
    Tgt>::type
to(std::string_view src) {
  Tgt result{};
  detail::parseTo(src, result);
  return result;
}

/*
 * enum <-> anything
 */

template <class Tgt, class Src>
typename std::enable_if<
    std::is_enum<Src>::value && !std::is_same<Src, Tgt>::value &&
        !std::is_convertible<Tgt, std::string_view>::value,
    Tgt>::type
to(const Src& value) {
  return to<Tgt>(static_cast<typename std::underlying_type<Src>::type>(value));
}

template <class Tgt, class Src>
typename std::enable_if<
    std::is_enum<Tgt>::value && !std::is_same<Src, Tgt>::value &&
        !std::is_convertible<Src, std::string_view>::value,
    Tgt>::type
to(const Src& value) {
  return static_cast<Tgt>(to<typename std::underlying_type<Tgt>::type>(value));
}

/*
 * Ts... -> string
 */

template <class Tgt>
void toAppend(std::string* /* result */) {}

template <class T, class... Ts>
typename std::enable_if<sizeof...(Ts) >= 1>::type
toAppend(std::string* result, const T& v, const Ts&... vs) {
  toAppend(result, v);
  toAppend(result, vs...);
}

template <class... Ts>
std::string toString(const Ts&... vs) {
  std::string result;
  toAppend(&result, vs...);
  return result;
}

} // namespace crystal
