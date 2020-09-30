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

#include "crystal/storage/table/TableGroupConfig.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

bool TableGroupConfig::parse(const dynamic& root) {
  auto name = root.getDefault("name");
  if (name.empty()) {
    CRYSTAL_LOG(ERROR) << "miss name: " << toCson(root);
    return false;
  }
  name_ = name.getString();
  auto version = root.getDefault("version");
  if (version.empty()) {
    CRYSTAL_LOG(ERROR) << "miss version: " << toCson(root);
    return false;
  }
  version_ = version.getString();
  auto tables = root.getDefault("table");
  if (tables.empty()) {
    CRYSTAL_LOG(ERROR) << "miss table: " << toCson(root);
    return false;
  }
  for (auto& kv : tables.items()) {
    TableConfig config(kv.first.getString());
    if (!config.parse(kv.second)) {
      CRYSTAL_LOG(ERROR) << "parse table failed";
      return false;
    }
    tableConfigs_[config.name()] = config;
  }
  if (tableConfigs_.empty()) {
    CRYSTAL_LOG(ERROR) << "no table in tablegroup";
    return false;
  }
  if (!checkRelated()) {
    CRYSTAL_LOG(ERROR) << "check related field failed";
    return false;
  }
  return true;
}

const std::string& TableGroupConfig::name() const {
  return name_;
}

const std::string& TableGroupConfig::version() const {
  return version_;
}

const std::map<std::string, TableConfig>&
TableGroupConfig::tableConfigs() const {
  return tableConfigs_;
}

DataType TableGroupConfig::getRelatedFieldType(const FieldConfig& field) const {
  if (!field.isRelated()) {
    return DataType::UNKNOWN;
  }
  return tableConfigs_.at(field.related())
      .recordConfig().at(field.name()).type();
}

bool TableGroupConfig::checkRelated() {
  for (auto& kv1 : tableConfigs_) {
    auto& tableConfig = kv1.second;
    for (auto& kv2 : tableConfig.relatedTables()) {
      auto field = kv2.first;
      auto& table = kv2.second;
      auto it1 = tableConfigs_.find(table);
      if (it1 == tableConfigs_.end()) {
        CRYSTAL_LOG(ERROR) << "related table '" << table << "' not exist";
        return false;
      }
      auto& recordConfig = it1->second.recordConfig();
      auto it2 = recordConfig.find(field);
      if (it2 == recordConfig.end()) {
        CRYSTAL_LOG(ERROR) << "related field '" << field << "'"
              << " not in related table '" << table << "'";
        return false;
      }
    }
  }
  return true;
}

}  // namespace crystal
