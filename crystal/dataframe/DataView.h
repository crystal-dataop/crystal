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

#include "crystal/dataframe/DataViewIndex.h"
#include "crystal/dataframe/DocumentArray.h"
#include "crystal/dataframe/DynamicTable.h"
#include "crystal/dataframe/FieldIndexMeta.h"

namespace crystal {

class DataView : public DataViewIndex {
 public:
  DataView() {}
  DataView(std::unique_ptr<DocumentArray> base);

  virtual ~DataView() {}

  DataView(const DataView&) = delete;
  DataView& operator=(const DataView&) = delete;

  DataView(DataView&&) = default;
  DataView& operator=(DataView&&) = default;

  DocumentArray* getBaseTable();
  DynamicTable* getDynamicTable();

  const ExtendedTable* getObject() const;

  size_t getRowCount() const;
  size_t getColCount() const;
  size_t getTokenCount() const;

  const Document* getDoc(size_t i) const;

  ItemType getColType(size_t j) const;

  bool inBase(size_t j) const;
  bool isColSet(size_t j) const;

  bool isValidDoc(size_t i) const;

  template <class T>
  std::optional<T> get(size_t i, size_t j) const;
  template <class T>
  std::optional<T> get(size_t doc, const std::string& field) const;
  template <class T>
  std::optional<T> get(size_t doc, FieldKey& field) const;
  template <class T>
  std::optional<T> getBaseUnsafe(size_t i, size_t j) const;

  void appendField(const std::string& field, bool column = false);

  template <class T>
  bool set(size_t i, size_t j, const T& value);
  template <class T>
  bool set(size_t doc, const std::string& field, const T& value);
  template <class T>
  bool set(size_t doc, FieldKey& field, const T& value);

  void trim(const U32IndexArray& docIndex);

  bool merge(DataView& other);

  template <class FIdx>
  void initFieldIndexMeta();
  template <class FIdx>
  FIdx& fieldIndexMeta() const;

 private:
  DynamicTable dynamicTable_;
  std::unique_ptr<DocumentArray> base_;
  std::unique_ptr<FieldIndexMeta> fieldIndexMeta_;
};

//////////////////////////////////////////////////////////////////////

inline DocumentArray* DataView::getBaseTable() {
  return base_.get();
}

inline DynamicTable* DataView::getDynamicTable() {
  return &dynamicTable_;
}

inline const ExtendedTable* DataView::getObject() const {
  return base_ ? base_->object() : nullptr;
}

inline size_t DataView::getRowCount() const {
  return docIndex().size();
}

inline size_t DataView::getColCount() const {
  return fieldIndex().size();
}

inline size_t DataView::getTokenCount() const {
  return base_ ? base_->getTokenCount() : 0;
}

inline const Document* DataView::getDoc(size_t i) const {
  return base_->getDoc(i);
}

inline ItemType DataView::getColType(size_t j) const {
  if (inBase(j)) {
    return base_->getColType(j);
  }
  return dynamicTable_.getColType(j);
}

inline bool DataView::inBase(size_t j) const {
  return base_ && j < base_->getFieldCount() && !dynamicTable_.isColSet(j);
}

inline bool DataView::isColSet(size_t j) const {
  return (base_ && j < base_->getFieldCount()) || dynamicTable_.isColSet(j);
}

inline bool DataView::isValidDoc(size_t i) const {
  if (base_) {
    auto* doc = getDoc(i);
    return doc && *doc;
  }
  CRYSTAL_THROW(RuntimeError, "unsupport");
}

template <class T>
inline std::optional<T> DataView::get(size_t i, size_t j) const {
  if (inBase(j)) {
    return base_->get<T>(i, j);
  }
  return dynamicTable_.get<T>(i, j);
}

template <class T>
inline std::optional<T> DataView::get(
    size_t doc, const std::string& field) const {
  return get<T>(getIndexOfDoc(doc), getIndexOfField(field));
}

template <class T>
inline std::optional<T> DataView::get(size_t doc, FieldKey& field) const {
  return get<T>(getIndexOfDoc(doc), field.getIndex(*this));
}

template <class T>
inline std::optional<T> DataView::getBaseUnsafe(size_t i, size_t j) const {
  return base_ ? base_->getUnsafe<T>(i, j) : std::nullopt;
}

inline void DataView::appendField(const std::string& field, bool column) {
  dynamicTable_.appendField(field, column);
  fieldIndex().append(field);
}

template <class T>
inline bool DataView::set(size_t i, size_t j, const T& value) {
  return dynamicTable_.set(i, j, value);
}

template <class T>
inline bool DataView::set(
    size_t doc, const std::string& field, const T& value) {
  return dynamicTable_.set(getIndexOfDoc(doc), getIndexOfField(field), value);
}

template <class T>
inline bool DataView::set(size_t doc, FieldKey& field, const T& value) {
  return dynamicTable_.set(getIndexOfDoc(doc), field.getIndex(*this), value);
}

template <class FIdx>
inline void DataView::initFieldIndexMeta() {
  fieldIndexMeta_ = std::make_unique<FIdx>();
}

template <class FIdx>
inline FIdx& DataView::fieldIndexMeta() const {
  return *reinterpret_cast<FIdx*>(fieldIndexMeta_.get());
}

//////////////////////////////////////////////////////////////////////

inline size_t FieldIndexMeta::Meta::getIndex(const DataView& view) {
  if (CRYSTAL_UNLIKELY(index_ == size_t(-1))) {
    index_ = view.getIndexOfField(name_);
  }
  return index_;
}

}  // namespace crystal
