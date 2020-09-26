/*
 * Copyright 2020 Yeolar
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
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace crystal {

template <class T>
struct IsBool: std::false_type {};

template <> struct IsBool<bool> : std::true_type {};

template <class T>
struct IsInt: std::false_type {};

template <> struct IsInt<int8_t> : std::true_type {};
template <> struct IsInt<int16_t> : std::true_type {};
template <> struct IsInt<int32_t> : std::true_type {};
template <> struct IsInt<int64_t> : std::true_type {};
template <> struct IsInt<uint8_t> : std::true_type {};
template <> struct IsInt<uint16_t> : std::true_type {};
template <> struct IsInt<uint32_t> : std::true_type {};
template <> struct IsInt<uint64_t> : std::true_type {};
#if defined(__APPLE__)
template <> struct IsInt<size_t> : std::true_type {};
#endif

template <class T>
struct IsFloat: std::false_type {};

template <> struct IsFloat<float> : std::true_type {};
template <> struct IsFloat<double> : std::true_type {};

template <class T>
struct IsString: std::false_type {};

template <> struct IsString<std::string> : std::true_type {};
template <> struct IsString<std::string_view> : std::true_type {};

template <class T>
struct IsValue {
  enum {
    value = IsBool<T>::value
         || IsInt<T>::value
         || IsFloat<T>::value
         || IsString<T>::value
  };
};

template <class T>
struct IsSequence : std::false_type {};

template <class T, size_t N>
struct IsSequence<std::array<T, N>> : std::true_type {};
template <class... Args>
struct IsSequence<std::deque<Args...>> : std::true_type {};
template <class... Args>
struct IsSequence<std::forward_list<Args...>> : std::true_type {};
template <class... Args>
struct IsSequence<std::list<Args...>> : std::true_type {};
template <class... Args>
struct IsSequence<std::vector<Args...>> : std::true_type {};

template <class T>
struct IsSet : std::false_type {};

template <class... Args>
struct IsSet<std::set<Args...>> : std::true_type {};
template <class... Args>
struct IsSet<std::unordered_set<Args...>> : std::true_type {};

template <class T>
struct IsMap : std::false_type {};

template <class... Args>
struct IsMap<std::map<Args...>> : std::true_type {};
template <class... Args>
struct IsMap<std::unordered_map<Args...>> : std::true_type {};

template <class T>
struct IsContainer {
  enum {
    value = IsSequence<T>::value || IsSet<T>::value
  };
};

template <class T, class Enable = void>
struct ContainerValueType {
  using type = void;
};

template <class T>
struct ContainerValueType<
    T, typename std::enable_if<IsContainer<T>::value>::type> {
  using type = typename T::value_type;
};

} // namespace crystal
