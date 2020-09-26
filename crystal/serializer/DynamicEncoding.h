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

#include <string_view>
#include <tuple>

#include "crystal/foundation/Logging.h"
#include "crystal/foundation/Traits.h"
#include "crystal/foundation/json.h"
#include "crystal/type/TypeTraits.h"

namespace crystal {

template <class T>
inline typename std::enable_if<IsBool<T>::value>::type
checkType(const dynamic& j) {
  CRYSTAL_CHECK(j.isBool()) << j;
}

template <class T>
inline typename std::enable_if<IsInt<T>::value>::type
checkType(const dynamic& j) {
  CRYSTAL_CHECK(j.isInt()) << j;
}

template <class T>
inline typename std::enable_if<IsFloat<T>::value>::type
checkType(const dynamic& j) {
  CRYSTAL_CHECK(j.isDouble()) << j;
}

template <class T>
inline typename std::enable_if<IsString<T>::value>::type
checkType(const dynamic& j) {
  CRYSTAL_CHECK(j.isString()) << j;
}

// IsValue
//
template <class T>
inline typename std::enable_if<IsValue<T>::value, dynamic>::type
encode(const T& value) {
  return value;
}

inline dynamic encode(const char* value) {
  return value;
}

inline void decode(const dynamic& j, bool& value) {
  checkType<bool>(j);
  value = j.getBool();
}

template <class T>
inline typename std::enable_if<IsInt<T>::value>::type
decode(const dynamic& j, T& value) {
  checkType<T>(j);
  value = j.getInt();
}

template <class T>
inline typename std::enable_if<IsFloat<T>::value>::type
decode(const dynamic& j, T& value) {
  checkType<T>(j);
  value = j.getDouble();
}

template <class T>
inline typename std::enable_if<IsString<T>::value>::type
decode(const dynamic& j, T& value) {
  checkType<T>(j);
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
inline typename std::enable_if<I == 0>::type
encodeImpl(dynamic& j, const std::tuple<Args...>& value) {
  typename std::tuple_element<0, std::tuple<Args...>>::type v;
  j[0] = encode(std::get<0>(value));
}

template <size_t I, class... Args>
inline typename std::enable_if<is_positive(I)>::type
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
inline typename std::enable_if<I == 0>::type
decodeImpl(const dynamic& j, std::tuple<Args...>& value) {
  typename std::tuple_element<0, std::tuple<Args...>>::type v;
  decode(j[0], std::get<0>(value));
}

template <size_t I, class... Args>
inline typename std::enable_if<is_positive(I)>::type
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

// IsSequence
//
template <class T>
typename std::enable_if<IsSequence<T>::value, dynamic>::type
encode(const T& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T>
typename std::enable_if<IsSequence<T>::value>::type
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
typename std::enable_if<IsSet<T>::value, dynamic>::type
encode(const T& value) {
  dynamic j = dynamic::array;
  for (auto& i : value) {
    j.push_back(encode(i));
  }
  return j;
}

template <class T>
typename std::enable_if<IsSet<T>::value>::type
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
typename std::enable_if<IsMap<T>::value, dynamic>::type
encode(const T& value) {
  dynamic j = dynamic::object;
  for (auto& kv : value) {
    j.insert(encode(kv.first), encode(kv.second));
  }
  return j;
}

template <class T>
typename std::enable_if<IsMap<T>::value>::type
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
