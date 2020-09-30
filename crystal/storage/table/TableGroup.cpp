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

#include "crystal/storage/table/TableGroup.h"

#include <filesystem>
#include <set>
#include <vector>

#include "crystal/foundation/Conv.h"
#include "crystal/foundation/Logging.h"
#include "crystal/foundation/String.h"

namespace fs = std::filesystem;

namespace crystal {

bool TableGroup::init(const char* dataDir, bool readOnly) {
  auto& tableConfigs = config_.tableConfigs();
  std::set<std::string> nameSet;
  for (auto& tableConfig : tableConfigs) {
    nameSet.insert(tableConfig.second.name());
  }
  if (!readOnly) {
    fs::create_directories(dataDir);
  }
  for (auto& dir : fs::directory_iterator(dataDir)) {
    fs::path path = dir;
    std::string name = path.filename();
    auto it = tableConfigs.find(name);
    if (it == tableConfigs.end()) {
      CRYSTAL_LOG(WARN) << "table '" << path << "' not found in conf";
      continue;
    }
    nameSet.erase(name);
    Table* table = new Table(it->second);
    if (!table->init(path.c_str(), readOnly)) {
      CRYSTAL_LOG(ERROR) << "init table '" << name << "' failed";
      return false;
    }
    tables_.emplace(name, std::unique_ptr<Table>(table));
  }
  if (!nameSet.empty()) {
    if (readOnly) {
      CRYSTAL_LOG(ERROR) << "miss tables: " << join(',', nameSet);
      return false;
    }
    for (auto& name : nameSet) {
      auto path = dataDir / fs::path(name);
      Table* table = new Table(tableConfigs.at(name));
      if (!table->init(path.c_str(), readOnly)) {
        CRYSTAL_LOG(ERROR) << "init table '" << name << "' failed";
        return false;
      }
      tables_.emplace(name, std::unique_ptr<Table>(table));
    }
  }
  CRYSTAL_LOG(INFO) << "init table group, directory: " << dataDir;
  return true;
}

void TableGroup::dump() {
  for (auto& kv : tables_) {
    kv.second->dump();
  }
}

}  // namespace crystal
