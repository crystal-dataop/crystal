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
#include <memory>

#include "crystal/storage/builder/TableGroupBuilder.h"
#include "crystal/storage/table/ExtendedTable.h"
#include "crystal/storage/table/TableGroup.h"

namespace crystal {

class TableFactory {
 public:
  TableFactory() {}
  virtual ~TableFactory() {}

  bool load(const char* confPath,
            const char* dataDir,
            bool readOnly,
            bool extendRelated = true);

  void drop(const std::string& name);

  void dump();

  TableGroup* getTableGroup(const std::string& name = "") const;
  TableGroupBuilder* getTableGroupBuilder(const std::string& name = "") const;

  ExtendedTable* getExtendedTable(const std::string& path) const;

 private:
  std::map<std::string, std::unique_ptr<TableGroup>> groups_;
  std::map<std::string, std::unique_ptr<TableGroupBuilder>> builders_;
  std::map<std::string, std::unique_ptr<ExtendedTable>> tables_;
};

}  // namespace crystal
