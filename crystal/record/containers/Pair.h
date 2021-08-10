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

#include <utility>

namespace crystal {

template <class T1, class T2>
struct pair {
  using first_type = T1;
  using second_type = T2;

  constexpr pair() = default;

  constexpr pair(const T1& x, const T2& y) : first(x), second(y) {}
  template <class U1, class U2>
  constexpr pair(U1&& x, U2&& y)
      : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

  template <class U1, class U2>
  constexpr pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
  template <class U1, class U2>
  constexpr pair(pair<U1, U2>&& p)
      : first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) {}

  pair(const pair& p) = default;
  pair(pair&& p) = default;

  pair& operator=(const pair& other) = default;
  pair& operator=(pair&& other) = default;

  template <class U1, class U2>
  pair& operator=(const pair<U1, U2>& other) {
    first = other.first;
    second = other.second;
    return *this;
  }
  template <class U1, class U2>
  pair& operator=(pair<U1, U2>&& other) {
    first = std::forward<U1>(other.first);
    second = std::forward<U2>(other.second);
    return *this;
  }

  T1 first;
  T2 second;
};

} // namespace crystal
