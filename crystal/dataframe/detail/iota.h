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

#include <numeric>

#include "crystal/foundation/Array.h"

namespace crystal {
namespace detail {

template <class T>
struct preinitarray_make_item {
  constexpr T operator()(std::size_t index) const {
    return index;
  }
};

constexpr size_t cPreInitArraySize = 10000;

template <class T>
constexpr std::array<T, cPreInitArraySize> cPreInitArray =
    make_array_with<cPreInitArraySize>(preinitarray_make_item<T>{});

template <class T>
void iota(T* b, T* e, size_t v) {
  size_t n = e - b;
  if (v >= cPreInitArraySize) {
    std::iota(b, e, v);
  } else if (n + v <= cPreInitArraySize) {
    memcpy(b, cPreInitArray<T>.data() + v, n * sizeof(T));
  } else {
    n = cPreInitArraySize - v;
    memcpy(b, cPreInitArray<T>.data() + v, n * sizeof(T));
    std::iota(b + n, e, cPreInitArraySize);
  }
}

}  // namespace detail
}  // namespace crystal
