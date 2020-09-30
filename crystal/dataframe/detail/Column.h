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

#include <optional>

#include "crystal/dataframe/DoubleLayerArray.h"
#include "crystal/dataframe/NumericIndexArray.h"
#include "crystal/foundation/dynamic.h"
#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {

struct Column {
  template <class T>
  std::optional<T> get(size_t i) const {
    if (i >= data_.size() || data_[i].isNull()) {
      return std::nullopt;
    }
    T value;
    decode(data_[i], value);
    return value;
  }

  template <class T>
  void set(size_t i, const T& value) {
    if (i >= data_.size()) {
      data_.resize(i);
      data_.emplace(encode(value));
    } else {
      data_[i] = encode(value);
    }
  }

  void trim(const U32IndexArray& docIndex);

  void merge(Column& other);

 private:
  DoubleLayerArray<dynamic> data_;
};

} // namespace crystal
