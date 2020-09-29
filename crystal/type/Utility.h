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

#include <limits>
#include <unordered_map>
#include <unordered_set>

#include "crystal/foundation/Conv.h"
#include "crystal/foundation/Demangle.h"
#include "crystal/foundation/String.h"
#include "crystal/foundation/Traits.h"
#include "crystal/foundation/dynamic.h"
#include "crystal/type/TypeTraits.h"

namespace crystal {

constexpr size_t npos = std::numeric_limits<size_t>::max();

template <class T, class Container>
typename std::enable_if<
  IsContainer<Container>::value &&
  std::is_same<T, typename ContainerValueType<Container>::type>::value,
  const Container&>::type
toVector(const Container& value) {
  return value;
}

template <class T, class Container>
typename std::enable_if<
  IsContainer<Container>::value &&
  !std::is_same<T, typename ContainerValueType<Container>::type>::value,
  std::vector<T>>::type
toVector(const Container& container) {
  std::vector<T> v;
  for (auto& i : container) {
    v.push_back(to<T>(i));
  }
  return v;
}

template <class T>
std::vector<T> toVector(std::string_view sv) {
  std::vector<T> v;
  split(',', sv, v, true);
  return v;
}

namespace detail {

CRYSTAL_CREATE_HAS_MEMBER_FN_TRAITS(has_toDynamic, toDynamic);

} // namespace detail

template <class T>
typename std::enable_if<
    detail::has_toDynamic<T, dynamic() const>::value, dynamic>::type
toDynamic(const T& object) {
  return object.toDynamic();
}

template <class T>
typename std::enable_if<
    !detail::has_toDynamic<T, dynamic() const>::value, dynamic>::type
toDynamic(const T&) {
  return demangle(typeid(T));
}

} // namespace crystal
