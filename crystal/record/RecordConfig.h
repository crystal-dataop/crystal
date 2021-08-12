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
#include <vector>

#include "crystal/foundation/json.h"
#include "crystal/record/containers/UnTypedTuple.h"
#include "crystal/type/DataType.h"

namespace crystal {

class FieldConfig {
 public:
  FieldConfig() {}

  bool parse(const dynamic& root);

  const std::string& name() const;
  DataType type() const;
  size_t count() const;
  const dynamic& dflt() const;
  const std::vector<FieldConfig>& tuple() const;
  const std::string& related() const;

  bool isRelated() const;

  dynamic toDynamic() const;
  std::string toString() const;

 private:
  std::string name_;
  DataType type_{DataType::UNKNOWN};
  size_t count_{1};
  dynamic default_;
  std::vector<FieldConfig> tuple_;
  std::string related_;
};

struct RecordConfig {
  std::vector<FieldConfig> fieldConfigs;
  std::map<std::string, size_t> fieldIndex;

  std::vector<FieldConfig> collect(
      const std::string& fields, bool addId = false);
  untyped_tuple::meta buildRecordMeta(
      const std::string& fields, bool addId = false);
};

RecordConfig parseRecordConfig(const dynamic& root);

} // namespace crystal
