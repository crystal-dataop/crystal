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

#include <array>
#include <type_traits>

#include "crystal/foundation/Conv.h"

namespace crystal {

namespace detail {

// Map from the character code to the hex value, or 16 if invalid hex char.
extern const std::array<unsigned char, 256> hexTable;

} // namespace detail

template <class InputString, class OutputString>
bool hexlify(const InputString& input, OutputString& output,
             bool append_output) {
  if (!append_output) {
    output.clear();
  }

  static char hexValues[] = "0123456789abcdef";
  auto j = output.size();
  output.resize(2 * input.size() + output.size());
  for (size_t i = 0; i < input.size(); ++i) {
    int ch = input[i];
    output[j++] = hexValues[(ch >> 4) & 0xf];
    output[j++] = hexValues[ch & 0xf];
  }
  return true;
}

template <class InputString, class OutputString>
bool unhexlify(const InputString& input, OutputString& output) {
  if (input.size() % 2 != 0) {
    return false;
  }
  output.resize(input.size() / 2);
  int j = 0;

  for (size_t i = 0; i < input.size(); i += 2) {
    int highBits = detail::hexTable[static_cast<uint8_t>(input[i])];
    int lowBits = detail::hexTable[static_cast<uint8_t>(input[i + 1])];
    if ((highBits | lowBits) & 0x10) {
      // One of the characters wasn't a hex digit
      return false;
    }
    output[j++] = (highBits << 4) + lowBits;
  }
  return true;
}

namespace detail {

/*
 * The following functions are type-overloaded helpers for
 * internalSplit().
 */
inline size_t delimSize(char)               { return 1; }
inline size_t delimSize(std::string_view s) { return s.size(); }
inline bool atDelim(const char* s, char c) {
  return *s == c;
}
inline bool atDelim(const char* s, std::string_view sv) {
  return !std::memcmp(s, sv.begin(), sv.size());
}

// These are used to short-circuit internalSplit() in the case of
// 1-character strings.
inline char delimFront(char c) {
  // This one exists only for compile-time; it should never be called.
  std::abort();
  return c;
}
inline char delimFront(std::string_view s) {
  assert(!s.empty() && s.begin() != nullptr);
  return *s.begin();
}

template <class OutStringT, class DelimT, class OutputIterator>
void internalSplit(DelimT delim, std::string_view sv, OutputIterator out,
    bool ignoreEmpty) {
  assert(sv.empty() || sv.begin() != nullptr);

  const char* s = sv.begin();
  const size_t strSize = sv.size();
  const size_t dSize = delimSize(delim);

  if (dSize > strSize || dSize == 0) {
    if (!ignoreEmpty || strSize > 0) {
      *out++ = to<OutStringT>(sv);
    }
    return;
  }
  if (std::is_same<DelimT, std::string_view>::value && dSize == 1) {
    // Call the char version because it is significantly faster.
    return internalSplit<OutStringT>(delimFront(delim), sv, out,
      ignoreEmpty);
  }

  size_t tokenStartPos = 0;
  size_t tokenSize = 0;
  for (size_t i = 0; i <= strSize - dSize; ++i) {
    if (atDelim(&s[i], delim)) {
      if (!ignoreEmpty || tokenSize > 0) {
        *out++ = to<OutStringT>(sv.substr(tokenStartPos, tokenSize));
      }

      tokenStartPos = i + dSize;
      tokenSize = 0;
      i += dSize - 1;
    } else {
      ++tokenSize;
    }
  }
  tokenSize = strSize - tokenStartPos;
  if (!ignoreEmpty || tokenSize > 0) {
    *out++ = to<OutStringT>(sv.substr(tokenStartPos, tokenSize));
  }
}

template <class String> std::string_view prepareDelim(const String& s) {
  return std::string_view(s);
}
inline char prepareDelim(char c) { return c; }

template <bool exact, class Delim, class OutputType>
bool splitFixed(
    const Delim& delimiter,
    std::string_view input,
    OutputType& output) {
  static_assert(
      exact || std::is_same<OutputType, std::string>::value ||
          std::is_same<OutputType, std::string_view>::value,
      "split<false>() requires that the last argument be a string type");
  if (exact && CRYSTAL_UNLIKELY(std::string::npos != input.find(delimiter))) {
    return false;
  }
  output = to<OutputType>(input);
  return true;
}

template <bool exact, class Delim, class OutputType, class... OutputTypes>
bool splitFixed(
    const Delim& delimiter,
    std::string_view input,
    OutputType& outHead,
    OutputTypes&... outTail) {
  size_t cut = input.find(delimiter);
  if (CRYSTAL_UNLIKELY(cut == std::string::npos)) {
    return false;
  }
  auto head = make_string_view(input.begin(), input.begin() + cut);
  auto tail = make_string_view(
      input.begin() + cut + detail::delimSize(delimiter), input.end());
  if (CRYSTAL_LIKELY(splitFixed<exact>(delimiter, tail, outTail...))) {
    outHead = to<OutputType>(head);
    return true;
  }
  return false;
}

} // namespace detail

template <class Delim, class String, class OutputType>
void split(const Delim& delimiter,
           const String& input,
           std::vector<OutputType>& out,
           bool ignoreEmpty) {
  detail::internalSplit<OutputType>(
    detail::prepareDelim(delimiter),
    std::string_view(input),
    std::back_inserter(out),
    ignoreEmpty);
}

template <
    class OutputValueType,
    class Delim,
    class String,
    class OutputIterator>
void splitTo(const Delim& delimiter,
             const String& input,
             OutputIterator out,
             bool ignoreEmpty) {
  detail::internalSplit<OutputValueType>(
    detail::prepareDelim(delimiter),
    std::string_view(input),
    out,
    ignoreEmpty);
}

template <bool exact, class Delim, class... OutputTypes>
typename std::enable_if<sizeof...(OutputTypes) >= 2, bool>::type
split(const Delim& delimiter, std::string_view input, OutputTypes&... outputs) {
  return detail::splitFixed<exact>(
      detail::prepareDelim(delimiter), input, outputs...);
}


namespace detail {

template <class T> struct IsSizableString {
  enum { value = std::is_same<T, std::string>::value
         || std::is_same<T, std::string_view>::value };
};

template <class Iterator>
struct IsSizableStringContainerIterator :
  IsSizableString<typename std::iterator_traits<Iterator>::value_type> {
};

template <class Delim, class Iterator>
void internalJoinAppend(Delim delimiter,
                        Iterator begin,
                        Iterator end,
                        std::string& output) {
  assert(begin != end);
  if (std::is_same<Delim, std::string_view>::value &&
      delimSize(delimiter) == 1) {
    internalJoinAppend(delimFront(delimiter), begin, end, output);
    return;
  }
  toAppend(&output, *begin);
  while (++begin != end) {
    toAppend(&output, delimiter, *begin);
  }
}

template <class Delim, class Iterator>
typename std::enable_if<IsSizableStringContainerIterator<Iterator>::value>::type
internalJoin(Delim delimiter,
             Iterator begin,
             Iterator end,
             std::string& output) {
  output.clear();
  if (begin == end) {
    return;
  }
  const size_t dsize = delimSize(delimiter);
  Iterator it = begin;
  size_t size = it->size();
  while (++it != end) {
    size += dsize + it->size();
  }
  output.reserve(size);
  internalJoinAppend(delimiter, begin, end, output);
}

template <class Delim, class Iterator>
typename
std::enable_if<!IsSizableStringContainerIterator<Iterator>::value>::type
internalJoin(Delim delimiter,
             Iterator begin,
             Iterator end,
             std::string& output) {
  output.clear();
  if (begin == end) {
    return;
  }
  internalJoinAppend(delimiter, begin, end, output);
}

} // namespace detail

template <class Delim, class Iterator>
void join(const Delim& delimiter,
          Iterator begin,
          Iterator end,
          std::string& output) {
  detail::internalJoin(
    detail::prepareDelim(delimiter),
    begin,
    end,
    output);
}

} // namespace crystal
