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

#include "crystal/storage/table/TableConfig.h"

#include "crystal/foundation/Exception.h"
#include "crystal/foundation/Logging.h"

namespace crystal {

bool TableConfig::parse(const dynamic& root) {
  recordConfig_ = parseRecordConfig(root);
  if (recordConfig_.empty()) {
    CRYSTAL_LOG(ERROR) << "parse record failed";
    return false;
  }
  for (auto& kv : recordConfig_) {
    if (kv.second.isRelated()) {
      relatedTables_[kv.first] = kv.second.related();
    }
  }
  if (!kvConfig_.parse(root, recordConfig_)) {
    return false;
  }
  auto indexes = root.getDefault("index");
  if (!indexes.empty()) {
    if (!indexes.isArray()) {
      CRYSTAL_LOG(ERROR) << "";
      return false;
    }
    for (auto& index : indexes) {
      IndexConfig indexConfig;
      if (!indexConfig.parse(index, recordConfig_)) {
        return false;
      }
      indexConfigs_[indexConfig.key()] = std::move(indexConfig);
    }
  }
  return true;
}

const std::string& TableConfig::name() const {
  return name_;
}

const RecordConfig& TableConfig::recordConfig() const {
  return recordConfig_;
}

const KVConfig& TableConfig::kvConfig() const {
  return kvConfig_;
}

const std::map<std::string, IndexConfig>& TableConfig::indexConfigs() const {
  return indexConfigs_;
}

const IndexConfig& TableConfig::indexConfig(const std::string& key) const {
  auto it = indexConfigs_.find(key);
  if (it != indexConfigs_.end()) {
    return it->second;
  }
  CRYSTAL_THROW(OutOfRange, "'", key, "' not in index list");
}

const std::map<std::string, std::string>& TableConfig::relatedTables() const {
  return relatedTables_;
}

}  // namespace crystal
