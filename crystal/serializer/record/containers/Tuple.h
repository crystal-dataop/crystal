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

template <size_t N, class Head, class... Tail>
struct tuple_impl : public tuple_impl<N + sizeof(Head), Tail...> {
  tuple_impl() = default;
  tuple_impl(Head&& first, Tail&&... tail)
      : tuple_impl<N + sizeof(Head), Tail...>{std::forward<Tail>(tail)...} {
    head() = std::forward<Head>(first);
  }

  Head& head() {
    return *reinterpret_cast<Head*>(
        *reinterpret_cast<OffsetPtr<uint8_t>*>(this) + N);
  }
};

template <size_t N, class Head>
struct tuple_impl<N, Head> {
  tuple_impl() = default;
  explicit tuple_impl(Head&& first) {
    head() = std::forward<Head>(first);
  }

  Head& head() { return *reinterpret_cast<Head*>(offset_ + N); }

  OffsetPtr<uint8_t> offset_;
  OffsetPtr<untyped_tuple::Meta> meta_;
};

template <class... Args>
using tuple = tuple_impl<0, Args...>;

template <class Tuple>
struct is_tuple : std::false_type {};

template <class... T>
struct is_tuple<tuple<T...>> : std::true_type {};

template <class T>
inline constexpr auto is_tuple_v = is_tuple<T>::value;

template <size_t I, size_t N, class Head, class... Tail,
          std::enable_if_t<I == 0U, int> = 0>
Head& get(tuple_impl<N, Head, Tail...>& t) {
  return t.head();
}

template <size_t I, size_t N, class Head, class... Tail,
          std::enable_if_t<I != 0U, int> = 0>
auto& get(tuple_impl<N, Head, Tail...>& t) {
  return get<I - 1U, N + sizeof(Head), Tail...>(t);
}

template <size_t I, class... Args>
auto& get(tuple<Args...>& t) {
  return get<I, 0, Args...>(t);
}

template <size_t I, size_t N, class Head, class... Tail,
          std::enable_if_t<I == 0U, int> = 0>
Head const& get(tuple_impl<N, Head, Tail...> const& t) {
  return t.head();
}

template <size_t I, size_t N, class Head, class... Tail,
          std::enable_if_t<I != 0U, int> = 0>
auto const& get(tuple_impl<N, Head, Tail...> const& t) {
  return get<I - 1U, N + sizeof(Head), Tail...>(t);
}

template <size_t I, class... Args>
auto const& get(tuple<Args...> const& t) {
  return get<I, 0, Args...>(t);
}

template <size_t I, size_t N, class Head, class... Tail,
          std::enable_if_t<I == 0U, int> = 0>
Head&& get(tuple_impl<N, Head, Tail...>&& t) {
  return t.head();
}

template <size_t I, size_t N, class Head, class... Tail,
          std::enable_if_t<I != 0U, int> = 0>
auto&& get(tuple_impl<N, Head, Tail...>&& t) {
  return get<I - 1U, N + sizeof(Head), Tail...>(t);
}

template <size_t I, class... Args>
auto&& get(tuple<Args...>&& t) {
  return get<I, 0, Args...>(t);
}

template <class T>
struct tuple_size;

template <class... T>
struct tuple_size<tuple<T...>>
    : public std::integral_constant<std::size_t, sizeof...(T)> {};

template <class T>
inline constexpr std::size_t tuple_size_v = tuple_size<std::decay_t<T>>::value;

template <class F, class Tuple, std::size_t... I>
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

template <class F, class Tuple, std::size_t... I>
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

template <class Tuple, std::size_t... I>
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

template <class Tuple, std::size_t Index = 0U>
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
