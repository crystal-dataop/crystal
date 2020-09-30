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

#include "crystal/storage/table/TableConfig.h"

namespace crystal {

class TableGroupConfig {
 public:
  TableGroupConfig() {}

  bool parse(const dynamic& root);

  const std::string& name() const;
  const std::string& version() const;
  const std::map<std::string, TableConfig>& tableConfigs() const;

  DataType getRelatedFieldType(const FieldConfig& field) const;

 private:
  bool checkRelated();

  std::string name_;
  std::string version_;
  std::map<std::string, TableConfig> tableConfigs_;
};

}  // namespace crystal
