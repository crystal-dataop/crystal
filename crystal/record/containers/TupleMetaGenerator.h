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

#include "crystal/foundation/AtomicLinkedList.h"
#include "crystal/foundation/Singleton.h"
#include "crystal/record/containers/Array.h"
#include "crystal/record/containers/String.h"
#include "crystal/record/containers/Tuple.h"
#include "crystal/record/containers/Vector.h"

namespace crystal {

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generateTupleMeta();

namespace detail {

template <class T>
inline std::enable_if_t<std::is_arithmetic_v<T> || std::is_same_v<T, string>>
addElementTypeImpl(untyped_tuple::meta& meta) {
  meta.add_type<T>(untyped_tuple::meta{}, 1);
}

template <class T>
inline std::enable_if_t<is_array_v<T>>
addElementTypeImpl(untyped_tuple::meta& meta) {
  meta.add_type<typename T::value_type>(untyped_tuple::meta{}, array_size_v<T>);
}

template <class T>
inline std::enable_if_t<is_vector_v<T>>
addElementTypeImpl(untyped_tuple::meta& meta) {
  meta.add_type<typename T::value_type>(untyped_tuple::meta{}, 0);
}

template <class T>
inline std::enable_if_t<is_tuple_v<T>>
addElementTypeImpl(untyped_tuple::meta& meta) {
  meta.add_type<T>(generateTupleMeta<T>(), 1);
}

} // namespace detail

template <size_t I, class T,
          std::enable_if_t<I + 1 == tuple_size_v<T>, int> = 0>
void addElementType(untyped_tuple::meta& meta) {
  detail::addElementTypeImpl<typename tuple_element<I, T>::type>(meta);
}

template <size_t I, class T,
          std::enable_if_t<I + 1 != tuple_size_v<T>, int> = 0>
void addElementType(untyped_tuple::meta& meta) {
  detail::addElementTypeImpl<typename tuple_element<I, T>::type>(meta);
  addElementType<I + 1, T>(meta);
}

struct TupleMetaWrapper {
  untyped_tuple::meta meta;

  TupleMetaWrapper(untyped_tuple::meta meta) : meta(meta) {}
  TupleMetaWrapper(const TupleMetaWrapper& other) = delete;
  TupleMetaWrapper(TupleMetaWrapper&& other) {
    std::swap(meta.offset, other.meta.offset);
  }
  TupleMetaWrapper& operator=(const TupleMetaWrapper& other) = delete;
  TupleMetaWrapper& operator=(TupleMetaWrapper&& other) {
    std::swap(meta.offset, other.meta.offset);
    return *this;
  }

  ~TupleMetaWrapper() {
    meta.release();
  }
};

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generateTupleMeta() {
  untyped_tuple::meta meta;
  meta.reserve(tuple_size_v<T>);
  addElementType<0, T>(meta);
  Singleton<AtomicLinkedList<TupleMetaWrapper>>::get()
    .insertHead(TupleMetaWrapper{meta});
  return meta;
}

} // namespace crystal
