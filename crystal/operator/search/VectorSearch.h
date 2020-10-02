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

#include "crystal/math/Span.h"
#include "crystal/operator/Operator.h"
#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {
namespace op {

class VectorSearch : public Operator<VectorSearch> {
  int64_t n_;
  Span<float> x_;
  std::string key_;
  std::string appendDistanceField_;
  uint16_t segment_;
  int64_t k_;

 public:
  VectorSearch(int64_t n,
               Span<float> x,
               const std::string& key,
               const std::string& appendDistanceField,
               uint16_t segment,
               int64_t k)
      : n_(n),
        x_(x),
        key_(key),
        appendDistanceField_(appendDistanceField),
        segment_(segment),
        k_(k) {}

  DataView& compose(DataView& view) const;

  dynamic toDynamic() const;
};

inline VectorSearch vSearch(
    int64_t n,
    Span<float> x,
    const std::string& key,
    const std::string& appendDistanceField,
    uint16_t segment,
    int64_t k) {
  return VectorSearch(n, x, key, appendDistanceField, segment, k);
}

} // namespace op
} // namespace crystal
