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

#include <functional>
#include <type_traits>
#include <utility>

#include "crystal/serializer/record/OffsetPtr.h"
#include "crystal/serializer/record/containers/UnTypedTuple.h"

namespace crystal {

template <class... Args>
struct tuple;

template <class Tuple>
struct is_tuple : std::false_type {};

template <class... T>
struct is_tuple<tuple<T...>> : std::true_type {};

template <class T>
inline constexpr bool is_tuple_v = is_tuple<T>::value;

template <class T>
std::enable_if_t<is_tuple_v<T>, untyped_tuple::meta>
generateTupleMeta();

template <class... Args>
struct tuple {
  tuple() = default;
  tuple(Args&&... args) {
    untyped_tuple_.reset(generateTupleMeta<tuple<Args...>>());
    set<0>(std::forward<Args>(args)...);
  }

  untyped_tuple untyped_tuple_;

 private:
  template <size_t I, class Head,
            std::enable_if_t<I == sizeof...(Args) - 1, int> = 0>
  void set(Head&& head) {
    untyped_tuple_.get<Head>(I) = std::forward<Head>(head);
  }

  template <size_t I, class Head, class... Tail,
            std::enable_if_t<I != sizeof...(Args) - 1, int> = 0>
  void set(Head&& head, Tail&&... tail) {
    untyped_tuple_.get<Head>(I) = std::forward<Head>(head);
    set<I + 1>(std::forward<Tail>(tail)...);
  }
};

template <class... T>
struct DataTypeTraits<tuple<T...>> {
  enum {
    value = static_cast<int>(DataType::TUPLE)
  };
};

template <class T>
struct unwrap_refwrapper {
  using type = T;
};

template <class T>
struct unwrap_refwrapper<std::reference_wrapper<T>> {
  using type = T&;
};

template <class T>
using unwrap_decay_t =
    typename unwrap_refwrapper<typename std::decay<T>::type>::type;

template <class... Args>
constexpr tuple<unwrap_decay_t<Args>...> make_tuple(Args&&... args) {
  return tuple<unwrap_decay_t<Args>...>(std::forward<Args>(args)...);
}

template <class T>
struct tuple_size;

template <class... T>
struct tuple_size<tuple<T...>>
    : public std::integral_constant<size_t, sizeof...(T)> {};

template <class T>
inline constexpr size_t tuple_size_v = tuple_size<std::decay_t<T>>::value;

template <size_t I, class T>
struct tuple_element;

template <size_t I, class Head, class... Tail>
struct tuple_element<I, tuple<Head, Tail...>>
    : tuple_element<I - 1, tuple<Tail...>> {};

template <class Head, class... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
  using type = Head;
};

template <size_t I, class Tuple,
          class T = typename tuple_element<I, Tuple>::type>
T& get(Tuple& t) {
  return t.untyped_tuple_.template get<T>(I);
}

template <size_t I, class Tuple,
          class T = typename tuple_element<I, Tuple>::type>
T const& get(Tuple const& t) {
  return t.untyped_tuple_.template get<T>(I);
}

template <size_t I, class Tuple,
          class T = typename tuple_element<I, Tuple>::type>
T&& get(Tuple&& t) {
  return t.untyped_tuple_.template get<T>(I);
}

template <class F, class Tuple, size_t... I>
constexpr decltype(auto) apply_impl(
    std::index_sequence<I...>, F&& f, Tuple&& t) {
  return std::invoke(std::forward<F>(f), get<I>(std::forward<Tuple>(t))...);
}

template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t) {
  return apply_impl(std::make_index_sequence<tuple_size_v<Tuple>>{},
                    std::forward<F>(f),
                    std::forward<Tuple>(t));
}

template <class F, class Tuple, size_t... I>
constexpr decltype(auto) apply_impl(
    std::index_sequence<I...>, F&& f, Tuple&& a, Tuple&& b) {
  return (std::invoke(std::forward<F>(f),
                      get<I>(std::forward<Tuple>(a)),
                      get<I>(std::forward<Tuple>(b))),
          ...);
}

template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& a, Tuple&& b) {
  return apply_impl(
      std::make_index_sequence<tuple_size_v<std::remove_reference_t<Tuple>>>{},
      std::forward<F>(f),
      std::forward<Tuple>(a),
      std::forward<Tuple>(b));
}

template <class Tuple, size_t... I>
constexpr decltype(auto) eq(std::index_sequence<I...>, Tuple&& a, Tuple&& b) {
  return ((get<I>(std::forward<Tuple>(a)) == get<I>(std::forward<Tuple>(b))) &&
          ...);
}

template <class Tuple>
std::enable_if_t<is_tuple_v<std::decay_t<Tuple>>, bool>
operator==(Tuple&& a, Tuple&& b) {
  return eq(
      std::make_index_sequence<tuple_size_v<std::remove_reference_t<Tuple>>>{},
      std::forward<Tuple>(a),
      std::forward<Tuple>(b));
}

template <class Tuple>
std::enable_if_t<is_tuple_v<std::decay_t<Tuple>>, bool>
operator!=(Tuple&& a, Tuple&& b) {
  return !(a == b);
}

template <class Tuple, size_t Index = 0U>
bool lt(Tuple&& a, Tuple&& b) {
  if constexpr (Index == tuple_size_v<Tuple>) {
    return false;
  } else {
    if (get<Index>(std::forward<Tuple>(a)) <
        get<Index>(std::forward<Tuple>(b))) {
      return true;
    }
    if (get<Index>(std::forward<Tuple>(b)) <
        get<Index>(std::forward<Tuple>(a))) {
      return false;
    }
    return lt<Tuple, Index + 1>(std::forward<Tuple>(a), std::forward<Tuple>(b));
  }
}

template <class Tuple>
std::enable_if_t<is_tuple_v<std::decay_t<Tuple>>, bool>
operator<(Tuple&& a, Tuple&& b) {
  return lt(a, b);
}

template <class Tuple>
std::enable_if_t<is_tuple_v<std::decay_t<Tuple>>, bool>
operator<=(Tuple&& a, Tuple&& b) {
  return !(b < a);
}

template <class Tuple>
std::enable_if_t<is_tuple_v<std::decay_t<Tuple>>, bool>
operator>(Tuple&& a, Tuple&& b) {
  return b < a;
}

template <class Tuple>
std::enable_if_t<is_tuple_v<std::decay_t<Tuple>>, bool>
operator>=(Tuple&& a, Tuple&& b) {
  return !(a < b);
}

} // namespace crystal
