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

#include "crystal/storage/index/IndexConfig.h"
#include "crystal/storage/kv/KVConfig.h"

namespace crystal {

class TableConfig {
 public:
  TableConfig(const std::string& name = "")
      : name_(name),
        kvConfig_(name) {}

  bool parse(const dynamic& root);

  const std::string& name() const;
  const RecordConfig& recordConfig() const;
  const KVConfig& kvConfig() const;
  const std::map<std::string, IndexConfig>& indexConfigs() const;
  const IndexConfig& indexConfig(const std::string& key) const;
  const std::map<std::string, std::string>& relatedTables() const;

 private:
  std::string name_;
  RecordConfig recordConfig_;
  KVConfig kvConfig_;
  std::map<std::string, IndexConfig> indexConfigs_;
  std::map<std::string, std::string> relatedTables_;
};

}  // namespace crystal
