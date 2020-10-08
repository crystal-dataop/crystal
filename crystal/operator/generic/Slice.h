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

#include "crystal/operator/Operator.h"

namespace crystal {
namespace op {

class Slice : public Operator<Slice> {
  size_t i_;
  size_t j_;
  bool reverse_;

 public:
  Slice(size_t i, size_t j, bool reverse)
      : i_(i), j_(j), reverse_(reverse) {}

  DataView& compose(DataView& view) const;

  dynamic toDynamic() const;
};

inline Slice slice(size_t i, size_t j = npos, bool reverse = false) {
  return Slice(i, j, reverse);
}

class SliceV : public Operator<SliceV> {
  std::vector<size_t> v_;
  bool reverse_;

 public:
  SliceV(const std::vector<size_t>& v, bool reverse)
      : v_(v), reverse_(reverse) {}

  DataView& compose(DataView& view) const;

  dynamic toDynamic() const;
};

inline SliceV slice(const std::vector<size_t>& v, bool reverse = false) {
  return SliceV(v, reverse);
}

} // namespace op
} // namespace crystal
