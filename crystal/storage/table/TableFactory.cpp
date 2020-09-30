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

#include "crystal/storage/table/TableFactory.h"

#include "crystal/foundation/File.h"

namespace crystal {

bool TableFactory::load(
    const char* confPath,
    const char* dataDir,
    bool readOnly,
    bool extendRelated) {
  std::string confStr;
  readFile(confPath, confStr);
  TableGroupConfig conf;
  if (!conf.parse(parseCson(confStr))) {
    CRYSTAL_LOG(ERROR) << "init table group conf '" << confPath << "' failed";
    return false;
  }
  if (groups_.find(conf.name()) != groups_.end()) {
    CRYSTAL_LOG(ERROR) << "table group '" << conf.name() << "' from '"
      << dataDir << "' already loaded";
    return false;
  }
  TableGroup* tableGroup = new TableGroup(conf);
  if (!tableGroup->init(dataDir, readOnly)) {
    CRYSTAL_LOG(ERROR) << "init table group '" << conf.name() << "' from '"
      << dataDir << "' failed";
    return false;
  }
  groups_.emplace(conf.name(), std::unique_ptr<TableGroup>(tableGroup));
  if (!readOnly) {
    builders_.emplace(conf.name(),
                      std::make_unique<TableGroupBuilder>(tableGroup));
  }
  for (auto& table : tableGroup->getTables()) {
    auto path = conf.name() + "/" + table.second->config().name();
    if (tables_.find(path) != tables_.end()) {
      CRYSTAL_LOG(ERROR) << "duplicate table: " << path;
      return false;
    }
    tables_.emplace(
        path,
        std::make_unique<ExtendedTable>(table.second.get(),
                                        tableGroup,
                                        extendRelated));
  }
  return true;
}

void TableFactory::drop(const std::string& name) {
  groups_.erase(name);
}

void TableFactory::dump() {
  for (auto& kv : groups_) {
    kv.second->dump();
  }
}

TableGroup* TableFactory::getTableGroup(const std::string& name) const {
  auto it = name != "" ? groups_.find(name) : groups_.begin();
  if (it == groups_.end()) {
    return nullptr;
  }
  return it->second.get();
}

TableGroupBuilder*
TableFactory::getTableGroupBuilder(const std::string& name) const {
  auto it = name != "" ? builders_.find(name) : builders_.begin();
  if (it == builders_.end()) {
    return nullptr;
  }
  return it->second.get();
}

ExtendedTable* TableFactory::getExtendedTable(const std::string& path) const {
  auto it = tables_.find(path);
  if (it == tables_.end()) {
    return nullptr;
  }
  return it->second.get();
}

}  // namespace crystal
