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

#include "crystal/serializer/record/containers/Array.h"
#include "crystal/serializer/record/containers/String.h"
#include "crystal/serializer/record/containers/Tuple.h"
#include "crystal/serializer/record/containers/Vector.h"

namespace crystal {

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generate_tuple_meta();

template <class T>
inline std::enable_if_t<is_array_v<T>>
add_element_type(untyped_tuple::meta& meta) {
  meta.add_type<T::value_type>({}, array_size_v<T>);
}

template <class T>
inline std::enable_if_t<is_vector_v<T>>
add_element_type(untyped_tuple::meta& meta) {
  meta.add_type<T::value_type>({}, 0);
}

template <class T>
inline std::enable_if_t<is_tuple_v<T>>
add_element_type(untyped_tuple::meta& meta) {
  meta.add_type<T>(generate_tuple_meta<T>, 1);
}

template <class T>
inline std::enable_if_t<std::is_arithmetic_v<T> || std::is_same_v<T, string>>
add_element_type(untyped_tuple::meta& meta) {
  meta.add_type<T>({}, 1);
}

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generate_tuple_meta() {
  untyped_tuple::meta meta;
  meta.resize(tuple_size_v<Tuple>);
  for (size_t i = 0; i < tuple_size_v<Tuple>; ++i) {
    add_element_type<tuple_element<i, Tuple>::type>(meta);
  }
  return meta;
}

} // namespace crystal
