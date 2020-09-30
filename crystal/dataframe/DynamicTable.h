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
#include "crystal/dataframe/detail/Column.h"
#include "crystal/dataframe/detail/Item.h"
#include "crystal/foundation/Logging.h"
#include "crystal/foundation/dynamic.h"
#include "crystal/serializer/DynamicEncoding.h"
#include "crystal/type/DataType.h"

namespace crystal {

struct ItemType {
  DataType type{DataType::UNKNOWN};
  size_t count{size_t(-1)};

  ItemType() {}
  ItemType(DataType t, size_t n) : type(t), count(n) {}
};

const ItemType unknownItemType = {};

class DynamicTable {
 public:
  DynamicTable() {}
  virtual ~DynamicTable() {}

  DynamicTable(const DynamicTable&) = delete;
  DynamicTable& operator=(const DynamicTable&) = delete;

  DynamicTable(DynamicTable&&) = default;
  DynamicTable& operator=(DynamicTable&&) = default;

  void appendBlank(size_t fieldCount);
  void appendField(const std::string& field, bool columnized = false);

  bool isColSet(size_t j) const;

  ItemType getColType(size_t j) const;

  size_t getDocCount() const;

  template <class T>
  std::optional<T> get(size_t i, size_t j) const;

  template <class T>
  bool set(size_t i, size_t j, const T& value);

  void trim(const U32IndexArray& docIndex);

  bool merge(DynamicTable& other);

 private:
  struct ColumnMeta {
    DataType type{DataType::UNKNOWN};
    std::string name;
    bool columnized;
    size_t index;
  };

  friend bool operator==(const ColumnMeta& lhs, const ColumnMeta& rhs);

  template <class T>
  bool checkMeta(size_t i, size_t j);

  template <class T>
  void appendColumn(ColumnMeta& meta);

  std::vector<ColumnMeta> metas_;
  std::vector<Column> columns_;
  DoubleLayerArray<std::vector<Item>> rows_;
  size_t columnCountOfRows_{0};
  size_t docCount_{0};
};

//////////////////////////////////////////////////////////////////////

inline bool operator==(const DynamicTable::ColumnMeta& lhs,
                       const DynamicTable::ColumnMeta& rhs) {
  return lhs.type == rhs.type
      && lhs.columnized == rhs.columnized
      && lhs.index == rhs.index;
}

inline void DynamicTable::appendBlank(size_t fieldCount) {
  metas_.resize(fieldCount);
}

inline void DynamicTable::appendField(
    const std::string& field, bool columnized) {
  ColumnMeta meta;
  meta.name = field;
  meta.columnized = columnized;
  metas_.push_back(std::move(meta));
}

template <class T>
void DynamicTable::appendColumn(ColumnMeta& meta) {
  meta.type = getType<T>();
  if (meta.columnized && IsValue<T>::value) {
    columns_.push_back(Column());
    meta.index = columns_.size() - 1;
  } else {
    meta.columnized = false;
    meta.index = columnCountOfRows_++;
  }
}

template <class T>
bool DynamicTable::checkMeta(size_t i, size_t j) {
  if (j >= metas_.size()) {
    return false;
  }
  auto& meta = metas_[j];
  if (meta.type == DataType::UNKNOWN) {
    appendColumn<T>(meta);
  }
  if (!checkType<T>(meta.type)) {
    return false;
  }
  if (!meta.columnized) {
    if (i >= rows_.size()) {
      rows_.resize(i + 1);
    }
    if (meta.index >= rows_[i].size()) {
      rows_[i].resize(meta.index + 1);
    }
  }
  return true;
}

inline bool DynamicTable::isColSet(size_t j) const {
  if (j >= metas_.size()) {
    return false;
  }
  return metas_[j].type != DataType::UNKNOWN;
}

inline ItemType DynamicTable::getColType(size_t j) const {
  if (j >= metas_.size()) {
    return unknownItemType;
  }
  return { metas_[j].type, 0 };
}

inline size_t DynamicTable::getDocCount() const {
  return docCount_;
}

template <class T>
std::optional<T> DynamicTable::get(size_t i, size_t j) const {
  if (j >= metas_.size()) {
    return std::nullopt;
  }
  auto& meta = metas_[j];
  if (!checkType<T>(meta.type)) {
    return std::nullopt;
  }
  if (meta.columnized) {
    return columns_[meta.index].get<T>(i);
  } else {
    return rows_[i][meta.index].get<T>();
  }
}

template <class T>
bool DynamicTable::set(size_t i, size_t j, const T& value) {
  if (!checkMeta<T>(i, j)) {
    return false;
  }
  auto& meta = metas_[j];
  if (!checkType<T>(meta.type)) {
    CRYSTAL_LOG(ERROR) << "set unmatch type: " << dataTypeToString(getType<T>())
        << "!=" << dataTypeToString(meta.type);
    return false;
  }
  if (meta.columnized) {
    columns_[meta.index].set<T>(i, value);
  } else {
    rows_[i][meta.index].set<T>(value);
  }
  docCount_ = std::max(docCount_, i + 1);
  return true;
}

} // namespace crystal
