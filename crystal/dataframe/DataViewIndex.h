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

#include "crystal/dataframe/NumericIndexArray.h"
#include "crystal/storage/table/FieldIndex.h"

namespace crystal {

class DataViewIndex {
 public:
  DataViewIndex() {}
  virtual ~DataViewIndex() {}

  DataViewIndex(const DataViewIndex&) = delete;
  DataViewIndex& operator=(const DataViewIndex&) = delete;

  DataViewIndex(DataViewIndex&&) = default;
  DataViewIndex& operator=(DataViewIndex&&) = default;

  FieldIndex& fieldIndex();
  const FieldIndex& fieldIndex() const;

  size_t getIndexOfField(const std::string& field) const;

  U32IndexArray& docIndex();
  const U32IndexArray& docIndex() const;

  uint32_t getIndexOfDoc(size_t i) const;

  bool mergeIndex(DataViewIndex& other);

 private:
  FieldIndex fieldIndex_;
  U32IndexArray docIndex_;
};

//////////////////////////////////////////////////////////////////////

inline FieldIndex& DataViewIndex::fieldIndex() {
  return fieldIndex_;
}

inline const FieldIndex& DataViewIndex::fieldIndex() const {
  return fieldIndex_;
}

inline size_t DataViewIndex::getIndexOfField(const std::string& field) const {
  return fieldIndex_.getIndexOfField(field);
}

inline U32IndexArray& DataViewIndex::docIndex() {
  return docIndex_;
}

inline const U32IndexArray& DataViewIndex::docIndex() const {
  return docIndex_;
}

inline uint32_t DataViewIndex::getIndexOfDoc(size_t i) const {
  return docIndex_[i];
}

inline bool DataViewIndex::mergeIndex(DataViewIndex& other) {
  return fieldIndex_.merge(other.fieldIndex_)
      && docIndex_.merge(other.docIndex_, docIndex_.size());
}

}  // namespace crystal
