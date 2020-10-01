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

#include "crystal/dataframe/Document.h"
#include "crystal/dataframe/DoubleLayerArray.h"
#include "crystal/dataframe/DynamicTable.h"
#include "crystal/dataframe/NumericIndexArray.h"

namespace crystal {

typedef DoubleLayerArray<Document> DocStorageArray;

class DocumentArray {
 public:
  typedef Document doc_type;

  DocumentArray() {}
  explicit DocumentArray(const ExtendedTable* object)
      : object_(object) {}

  virtual ~DocumentArray() {}

  DocumentArray(const DocumentArray&) = delete;
  DocumentArray& operator=(const DocumentArray&) = delete;

  DocumentArray(DocumentArray&&) = default;
  DocumentArray& operator=(DocumentArray&&) = default;

  const ExtendedTable* object() const;

  DocStorageArray& docs();

  size_t getDocCount() const;
  size_t getFieldCount() const;
  size_t getTokenCount() const;

  void incrementTokenCount();

  const std::vector<std::string>& getFields() const;
  const FieldIndex& getFieldIndex() const;

  const Document* getDoc(size_t i) const;

  ItemType getColType(size_t j) const;

  template <class T>
  std::optional<T> get(size_t i, size_t j) const;
  template <class T>
  std::optional<T> getUnsafe(size_t i, size_t j) const;

  void trim(const U32IndexArray& docIndex);

  bool merge(DocumentArray& other);

  void copyTo(DynamicTable& table,
              const U32IndexArray& docIndex,
              const std::vector<size_t>& fieldIndex);

 private:
  const ExtendedTable* object_{nullptr};
  DocStorageArray docs_;
  size_t tokenCount_{0};
  uint16_t indexNo_{uint16_t(-1)};
};

//////////////////////////////////////////////////////////////////////

inline const ExtendedTable* DocumentArray::object() const {
  return object_;
}

inline DocStorageArray& DocumentArray::docs() {
  return docs_;
}

inline size_t DocumentArray::getDocCount() const {
  return docs_.size();
}

inline size_t DocumentArray::getFieldCount() const {
  return object_ ? object_->fields().size() : 0;
}

inline size_t DocumentArray::getTokenCount() const {
  return tokenCount_;
}

inline void DocumentArray::incrementTokenCount() {
  ++tokenCount_;
}

inline const std::vector<std::string>& DocumentArray::getFields() const {
  return object_->fields();
}

inline const FieldIndex& DocumentArray::getFieldIndex() const {
  return object_->fieldIndex();
}

inline const Document* DocumentArray::getDoc(size_t i) const {
  if (i >= getDocCount()) {
    return nullptr;
  }
  return &docs_[i];
}

inline ItemType DocumentArray::getColType(size_t j) const {
  if (j >= getFieldCount()) {
    return unknownItemType;
  }
  return {
    object_->fieldInfos()[j].meta.type(),
    object_->fieldInfos()[j].meta.count()
  };
}

template <class T>
inline std::optional<T> DocumentArray::get(size_t i, size_t j) const {
  if (i >= getDocCount() || j >= getFieldCount()) {
    return std::nullopt;
  }
  return getUnsafe<T>(i, j);
}

template <class T>
inline std::optional<T> DocumentArray::getUnsafe(size_t i, size_t j) const {
  return docs_[i].get<T>(j);
}

}  // namespace crystal
