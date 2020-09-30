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

#include "crystal/dataframe/DynamicTable.h"

namespace crystal {

void DynamicTable::trim(const U32IndexArray& docIndex) {
  for (auto& meta : metas_) {
    if (meta.columnized) {
      columns_[meta.index].trim(docIndex);
    }
  }
  DoubleLayerArray<std::vector<Item>> rows;
  for (auto i : docIndex) {
    if (i >= rows_.size()) {
      rows.emplace();
    } else {
      rows.emplace(std::move(rows_[i]));
    }
  }
  rows_.swap(rows);
}

bool DynamicTable::merge(DynamicTable& other) {
  if (metas_ != other.metas_) {
    return false;
  }
  for (auto& meta : metas_) {
    if (meta.columnized) {
      columns_[meta.index].merge(other.columns_[meta.index]);
    }
  }
  for (auto& row : other.rows_) {
    rows_.emplace(std::move(row));
  }
  return true;
}

} // namespace crystal
