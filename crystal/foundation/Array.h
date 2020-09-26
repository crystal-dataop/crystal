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

/*
 * Modified from folly.
 */

#pragma once

#include <array>
#include <utility>

namespace crystal {

namespace array_detail {

template <typename MakeItem, std::size_t... Index>
inline
__attribute__((__always_inline__)) __attribute__((__visibility__("hidden")))
constexpr auto make_array_with(
    MakeItem const& make,
    std::index_sequence<Index...>) {
  return std::array<decltype(make(0)), sizeof...(Index)>{{make(Index)...}};
}
} // namespace array_detail

//  make_array_with
//
//  Constructs a std::array<..., Size> with elements m(i) for i in [0, Size).
template <std::size_t Size, typename MakeItem>
constexpr auto make_array_with(MakeItem const& make) {
  return array_detail::make_array_with(make, std::make_index_sequence<Size>{});
}

} // namespace crystal
