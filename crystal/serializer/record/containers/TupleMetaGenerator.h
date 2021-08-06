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

#include <type_traits>

#include "crystal/serializer/record/containers/Array.h"
#include "crystal/serializer/record/containers/String.h"
#include "crystal/serializer/record/containers/Tuple.h"
#include "crystal/serializer/record/containers/Vector.h"

namespace crystal {

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generateTupleMeta();

template <class T>
inline std::enable_if_t<is_array_v<T>>
addElementType(untyped_tuple::meta& meta) {
  meta.add_type<T::value_type>({}, array_size_v<T>);
}

template <class T>
inline std::enable_if_t<is_vector_v<T>>
addElementType(untyped_tuple::meta& meta) {
  meta.add_type<T::value_type>({}, 0);
}

template <class T>
inline std::enable_if_t<is_tuple_v<T>>
addElementType(untyped_tuple::meta& meta) {
  meta.add_type<T>(generateTupleMeta<T>, 1);
}

template <class T>
inline std::enable_if_t<std::is_arithmetic_v<T> || std::is_same_v<T, string>>
addElementType(untyped_tuple::meta& meta) {
  meta.add_type<T>({}, 1);
}

template <size_t I, class T>
struct ElementTypeToMeta;

template <size_t I, class Head, class... Tail>
struct ElementTypeToMeta<I, tuple<Head, Tail...>>
    : ElementTypeToMeta<I - 1, tuple<Tail...>> {
  ElementTypeToMeta<I, tuple<Head, Tail...>>(untyped_tuple::meta& meta) {
    addElementType<Head>(meta);
  }
};

template <class Head, class... Tail>
struct ElementTypeToMeta<0, tuple<Head, Tail...>> {
  ElementTypeToMeta<0, tuple<Head, Tail...>>(untyped_tuple::meta& meta) {
    addElementType<Head>(meta);
  }
};

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generateTupleMeta() {
  untyped_tuple::meta meta;
  meta.resize(tuple_size_v<T>);
  ElementTypeToMeta<tuple_size_v<T> - 1, T> to(meta);
  return meta;
}

} // namespace crystal
