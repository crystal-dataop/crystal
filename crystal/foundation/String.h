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
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace crystal {

inline constexpr size_t constexpr_strlen(const char* s) {
#if defined(__clang__)
  // clang provides a constexpr builtin
  return __builtin_strlen(s);
#else
  // strlen() happens to already be constexpr under gcc
  return std::strlen(s);
#endif
}

std::string stringPrintf(const char* format, ...);
std::string stringVPrintf(const char* format, va_list ap);

std::string errnoStr(int err);

template <class Delim, class String, class OutputType>
void split(const Delim& delimiter,
           const String& input,
           std::vector<OutputType>& out,
           const bool ignoreEmpty = false);

template <
    class OutputValueType,
    class Delim,
    class String,
    class OutputIterator>
void splitTo(const Delim& delimiter,
             const String& input,
             OutputIterator out,
             const bool ignoreEmpty = false);

template <bool exact = true, class Delim, class... OutputTypes>
typename std::enable_if<sizeof...(OutputTypes) >= 2, bool>::type
split(const Delim& delimiter, std::string_view input, OutputTypes&... outputs);

template <class Delim, class Iterator>
void join(const Delim& delimiter,
          Iterator begin,
          Iterator end,
          std::string& output);

template <class Delim, class Container>
std::string join(const Delim& delimiter, const Container& container) {
  std::string output;
  join(delimiter, container.begin(), container.end(), output);
  return output;
}

void toLower(char* str, size_t length);

inline void toLower(std::string& str) {
  toLower(&str[0], str.size());
}

//////////////////////////////////////////////////////////////////////

template <class Iter>
constexpr std::string_view make_string_view(Iter begin, Iter end) {
  return std::string_view(begin, end - begin);
}

inline bool startsWith(std::string_view sv, char c) {
  return !sv.empty() && sv.front() == c;
}

inline bool startsWith(std::string_view sv, std::string_view prefix) {
  return sv.substr(0, prefix.size()) == prefix;
}

inline bool endsWith(std::string_view sv, char c) {
  return !sv.empty() && sv.back() == c;
}

inline bool endsWith(std::string_view sv, std::string_view suffix) {
  return sv.substr(sv.size() - suffix.size()) == suffix;
}

inline bool removePrefix(std::string_view& sv, std::string_view prefix) {
  return startsWith(sv, prefix) ?
    (sv.remove_prefix(prefix.size()), true) : false;
}

inline bool removeSuffix(std::string_view& sv, std::string_view suffix) {
  return endsWith(sv, suffix) ? (sv.remove_suffix(suffix.size()), true) : false;
}

std::string_view ltrimWhitespace(std::string_view sv);
std::string_view rtrimWhitespace(std::string_view sv);

inline std::string_view trimWhitespace(std::string_view sv) {
  return ltrimWhitespace(rtrimWhitespace(sv));
}

} // namespace crystal

#include "String-inl.h"
