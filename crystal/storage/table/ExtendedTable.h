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

#include <vector>

#include "crystal/storage/index/Index.h"
#include "crystal/storage/table/FieldIndex.h"
#include "crystal/storage/table/FieldInfo.h"
#include "crystal/storage/table/TableGroup.h"

namespace crystal {

class ExtendedTable {
 public:
  ExtendedTable(Table* table, TableGroup* tableGroup, bool extend = true);

  virtual ~ExtendedTable() {}

  Table* table() const;
  TableGroup* tableGroup() const;

  const std::string& name() const;
  const std::string& key() const;

  const std::vector<std::string>& fields() const;
  const std::vector<FieldInfo>& fieldInfos() const;
  const FieldIndex& fieldIndex() const;

  bool hasKV() const;
  DataType getFieldType(const std::string& field) const;

 private:
  void buildFieldInfo(const FieldMeta& meta,
                      int type,
                      const IndexNoSet* set = nullptr,
                      FieldInfo::Related related = FieldInfo::Related());

  Table* table_;
  TableGroup* tableGroup_;
  std::vector<std::string> fields_;
  std::vector<FieldInfo> fieldInfos_;
  FieldIndex fieldIndex_;
};

//////////////////////////////////////////////////////////////////////

inline Table* ExtendedTable::table() const {
  return table_;
}

inline TableGroup* ExtendedTable::tableGroup() const {
  return tableGroup_;
}

inline const std::string& ExtendedTable::name() const {
  return table_->config().name();
}

inline const std::string& ExtendedTable::key() const {
  return table_->config().kvConfig().key();
}

inline const std::vector<std::string>& ExtendedTable::fields() const {
  return fields_;
}

inline const std::vector<FieldInfo>& ExtendedTable::fieldInfos() const {
  return fieldInfos_;
}

inline const FieldIndex& ExtendedTable::fieldIndex() const {
  return fieldIndex_;
}

}  // namespace crystal
