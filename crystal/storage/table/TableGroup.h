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

#include <map>
#include <string>

#include "crystal/storage/table/Table.h"
#include "crystal/storage/table/TableGroupConfig.h"

namespace crystal {

class TableGroup {
 public:
  explicit TableGroup(const TableGroupConfig& config)
      : config_(config) {}

  virtual ~TableGroup() {}

  TableGroup(const TableGroup&) = delete;
  TableGroup& operator=(const TableGroup&) = delete;

  bool init(const char* dataDir, bool readOnly);

  void dump();

  const TableGroupConfig& config() const;

  Table* getTable(const std::string& name) const;

  const std::map<std::string, std::unique_ptr<Table>>& getTables() const;

 private:
  std::map<std::string, std::unique_ptr<Table>> tables_;
  TableGroupConfig config_;
};

//////////////////////////////////////////////////////////////////////

inline const TableGroupConfig& TableGroup::config() const {
  return config_;
}

inline Table* TableGroup::getTable(const std::string& name) const {
  auto it = tables_.find(name);
  if (it == tables_.end()) {
    return nullptr;
  }
  return it->second.get();
}

inline const std::map<std::string, std::unique_ptr<Table>>&
TableGroup::getTables() const {
  return tables_;
}

}  // namespace crystal
