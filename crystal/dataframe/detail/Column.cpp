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

#include "crystal/dataframe/detail/Column.h"

namespace crystal {

void Column::trim(const U32IndexArray& docIndex) {
  DoubleLayerArray<dynamic> data;
  for (auto i : docIndex) {
    data.emplace(data_[i]);
  }
  data_.swap(data);
}

void Column::merge(Column& other) {
  if (!other.data_.empty()) {
    for (auto i : other.data_) {
      data_.emplace(i);
    }
  }
}

} // namespace crystal
