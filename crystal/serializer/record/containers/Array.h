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

#include <cstddef>
#include <stdexcept>

namespace crystal {

template <class T, size_t N>
class array {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;

  array() noexcept = default;
  ~array() = default;

  array& operator=(const array&) = default;

  constexpr reference at(size_t pos) {
    if (pos >= size()) {
      throw std::out_of_range("array::at");
    }
    return elements_[pos];
  }
  constexpr const_reference at(size_t pos) const {
    if (pos >= size()) {
      throw std::out_of_range("array::at");
    }
    return elements_[pos];
  }

  constexpr reference operator[](size_t pos) {
    return elements_[pos];
  }
  constexpr const_reference operator[](size_t pos) const {
    return elements_[pos];
  }

  constexpr T& front() {
    return elements_[0];
  }
  constexpr const T& front() const {
    return elements_[0];
  }

  constexpr T& back() {
    return elements_[size() - 1];
  }
  constexpr const T& back() const {
    return elements_[size() - 1];
  }

  constexpr T* data() noexcept {
    return elements_;
  }
  constexpr const T* data() const noexcept {
    return elements_;
  }

  constexpr iterator begin() noexcept {
    return data();
  }
  constexpr const_iterator begin() const noexcept {
    return data();
  }

  constexpr iterator end() noexcept {
    return data() + size();
  }
  constexpr const_iterator end() const noexcept {
    return data() + size();
  }

  constexpr bool empty() const noexcept {
    return size() == 0;
  }

  constexpr size_t size() const noexcept {
    return N;
  }

  void fill(const T& value) {
    for (size_t i = 0; i < N; ++i) {
      elements_[i] = value;
    }
  }

 private:
  T elements_[N];
};

template <class Array>
struct is_array : std::false_type {};

template <class T, size_t N>
struct is_array<array<T, N>> : std::true_type {};

template <class T>
inline constexpr bool is_array_v = is_array<T>::value;

template <class T>
struct array_size;

template <class T, size_t N>
struct array_size<array<T, N>>
    : public std::integral_constant<size_t, N> {};

template <class T>
inline constexpr size_t array_size_v = array_size<std::decay_t<T>>::value;

} // namespace crystal
