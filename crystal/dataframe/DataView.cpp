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

#include "crystal/dataframe/DataView.h"

namespace crystal {

DataView::DataView(std::unique_ptr<DocumentArray> base)
    : base_(std::move(base)) {
  docIndex().resize(base_->getDocCount());
  fieldIndex() = base_->getFieldIndex();
  dynamicTable_.appendBlank(base_->getFieldCount());
}

void DataView::trim(const U32IndexArray& index) {
  if (base_) {
    base_->trim(index);
  }
  dynamicTable_.trim(index);
  U32IndexArray trimIndex;
  trimIndex.resize(index.size());
  docIndex().swap(trimIndex);
}

bool DataView::merge(DataView& other) {
  if (base_) {
    if (!base_->merge(*other.base_.get())) {
      return false;
    }
  }
  return dynamicTable_.merge(other.dynamicTable_) && mergeIndex(other);
}

} // namespace crystal
