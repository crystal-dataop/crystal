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

#include <string_view>
#include <tuple>
#include <type_traits>

#include "crystal/foundation/Logging.h"
#include "crystal/foundation/Traits.h"
#include "crystal/foundation/json.h"
#include "crystal/record/RecordBase.h"
#include "crystal/type/TypeTraits.h"

namespace crystal {

// IsValue
//
template <class T>
inline std::enable_if_t<IsValue<T>::value, dynamic>
encode(const T& value) {
  return value;
}

inline dynamic encode(const char* value) {
  return value;
}

inline void decode(const dynamic& j, bool& value) {
  value = j.getBool();
}

template <class T>
inline std::enable_if_t<IsInt<T>::value>
decode(const dynamic& j, T& value) {
  value = j.getInt();
}

template <class T>
inline std::enable_if_t<IsFloat<T>::value>
decode(const dynamic& j, T& value) {
  value = j.getDouble();
}

template <class T>
inline std::enable_if_t<IsString<T>::value>
decode(const dynamic& j, T& value) {
  value = j.getString();
}

// std::pair
//
template <class K, class V>
inline dynamic encode(const std::pair<K, V>& value) {
  return dynamic::object(
      encode(value.first),
      encode(value.second));
}

template <class K, class V>
inline void decode(const dynamic& j, std::pair<K, V>& value) {
  decode(j.items().begin()->first, value.first);
  decode(j.items().begin()->second, value.second);
}

namespace detail {

template <size_t I, class... Args>
inline std::enable_if_t<I == 0>
encodeImpl(dynamic& j, const std::tuple<Args...>& value) {
  typename std::tuple_element<0, std::tuple<Args...>>::type v;
  j[0] = encode(std::get<0>(value));
}

template <size_t I, class... Args>
inline std::enable_if_t<is_positive(I)>
encodeImpl(dynamic& j, const std::tuple<Args...>& value) {
  typename std::tuple_element<I, std::tuple<Args...>>::type v;
  j[I] = encode(std::get<I>(value));
  encodeImpl<I - 1>(j, value);
}

} // namespace detail

// std::tuple
//
template <class... Args>
inline dynamic encode(const std::tuple<Args...>& value) {
  dynamic j = dynamic::array;
  j.resize(sizeof...(Args));
  detail::encodeImpl<sizeof...(Args) - 1>(j, value);
  return j;
}

namespace detail {

template <size_t I, class... Args>
inline std::enable_if_t<I == 0>
decodeImpl(const dynamic& j, std::tuple<Args...>& value) {
  typename std::tuple_element<0, std::tuple<Args...>>::type v;
  decode(j[0], std::get<0>(value));
}

template <size_t I, class... Args>
inline std::enable_if_t<is_positive(I)>
decodeImpl(const dynamic& j, std::tuple<Args...>& value) {
  typename std::tuple_element<I, std::tuple<Args...>>::type v;
  decode(j[I], std::get<I>(value));
  decodeImpl<I - 1>(j, value);
}

} // namespace detail

template <class... Args>
inline void decode(const dynamic& j, std::tuple<Args...>& value) {
  detail::decodeImpl<sizeof...(Args) - 1>(j, value);
}

// std::array
//
template <class T, size_t N>
dynamic encode(const std::array<T, N>& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T, size_t N>
void decode(const dynamic& j, std::array<T, N>& value) {
  CRYSTAL_CHECK(j.size() == N) << j;
  for (size_t i = 0; i < N; ++i) {
    decode(j[i], value[i]);
  }
}

// IsSequence
//
template <class T>
std::enable_if_t<IsSequence<T>::value, dynamic>
encode(const T& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T>
std::enable_if_t<IsSequence<T>::value>
decode(const dynamic& j, T& value) {
  value.reserve(j.size());
  for (auto& i : j) {
    typename T::value_type v;
    decode(i, v);
    value.push_back(std::move(v));
  }
}

// IsSet
//
template <class T>
std::enable_if_t<IsSet<T>::value, dynamic>
encode(const T& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T>
std::enable_if_t<IsSet<T>::value>
decode(const dynamic& j, T& value) {
  for (auto& i : j) {
    typename T::value_type v;
    decode(i, v);
    value.insert(std::move(v));
  }
}

// IsMap
//
template <class T>
std::enable_if_t<IsMap<T>::value, dynamic>
encode(const T& value) {
  dynamic j = dynamic::object;
  for (auto& kv : value) {
    j.insert(encode(kv.first), encode(kv.second));
  }
  return j;
}

template <class T>
std::enable_if_t<IsMap<T>::value>
decode(const dynamic& j, T& value) {
  for (auto& kv : j.items()) {
    typename T::key_type k;
    typename T::mapped_type v;
    decode(kv.first, k);
    decode(kv.second, v);
    value.insert(std::make_pair(k, v));
  }
}

//////////////////////////////////////////////////////////////////////

inline dynamic encode(const string& value) {
  return std::string_view(value);
}

inline void decode(const dynamic& j, string& value) {
  value = j.getString();
}

template <class T, size_t N>
dynamic encode(const array<T, N>& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T, size_t N>
void decode(const dynamic& j, array<T, N>& value) {
  CRYSTAL_CHECK(j.size() == N) << j;
  for (size_t i = 0; i < N; ++i) {
    decode(j[i], value[i]);
  }
}

template <class T>
dynamic encode(const Range<T>& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T>
void decode(const dynamic& j, Range<T>& value) {
  CRYSTAL_CHECK(j.size() == value.size()) << j;
  for (size_t i = 0; i < value.size(); ++i) {
    decode(j[i], value[i]);
  }
}

template <class T>
dynamic encode(const vector<T>& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T>
void decode(const dynamic& j, vector<T>& value) {
  value.write(j.size(), [&](T* p, size_t) {
    T* it = p;
    for (auto& i : j) {
      T v;
      decode(i, v);
      new (it++) T(v);
    }
  });
}

dynamic encode(const untyped_tuple& value);

void decode(const dynamic& j, untyped_tuple& value);

inline dynamic encode(const RecordBase& value) {
  return encode(value.untypedTuple());
}

inline void decode(const dynamic& j, RecordBase& value) {
  decode(j, value.untypedTuple());
}

//////////////////////////////////////////////////////////////////////

namespace detail {

template <class T>
inline void vencodeImpl(dynamic& j, const T& arg) {
  j.push_back(encode(arg));
}

template <class T, class... Args>
inline void vencodeImpl(dynamic& j, const T& arg, const Args&... args) {
  j.push_back(encode(arg));
  vencodeImpl(j, args...);
}

template <class T>
inline void vdecodeImpl(const dynamic& j, int i, T& arg) {
  decode(j[i], arg);
}

template <class T, class... Args>
inline void vdecodeImpl(const dynamic& j, int i, T& arg, Args&... args) {
  decode(j[i], arg);
  vdecodeImpl(j, i + 1, args...);
}

} // namespace detail

template <class... Args>
inline dynamic vencode(const Args&... args) {
  dynamic j = dynamic::array;
  detail::vencodeImpl(j, args...);
  return j;
}

template <class... Args>
inline void vdecode(const dynamic& j, Args&... args) {
  detail::vdecodeImpl(j, 0, args...);
}

template <class T>
inline void vdecodeOne(const dynamic& j, int i, T& arg) {
  detail::vdecodeImpl(j, i, arg);
}

template <class... Args>
inline std::string serialize(const Args&... args) {
  return toJson(vencode(args...));
}

template <class... Args>
inline void unserialize(std::string_view sv, Args&... args) {
  vdecode(parseJson(sv), args...);
}

} // namespace crystal
