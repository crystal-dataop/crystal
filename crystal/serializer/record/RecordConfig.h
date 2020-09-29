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

#include "crystal/foundation/json.h"
#include "crystal/serializer/record/FieldMeta.h"
#include "crystal/serializer/record/RecordMeta.h"
#include "crystal/type/DataType.h"

namespace crystal {

class FieldConfig {
 public:
  FieldConfig() {}

  bool parse(const dynamic& root);

  const std::string& name() const;
  int tag() const;
  DataType type() const;
  size_t bits() const;
  size_t count() const;
  const dynamic& dflt() const;
  const std::string& related() const;

  bool isRelated() const;

  FieldMeta toFieldMeta() const;
  dynamic toDynamic() const;
  std::string toString() const;

 private:
  std::string name_;
  int tag_{0};
  DataType type_{DataType::UNKNOWN};
  size_t bits_{0};
  size_t count_{1};
  dynamic default_;
  std::string related_;
};

typedef std::map<std::string, FieldConfig> RecordConfig;

RecordConfig parseRecordConfig(const dynamic& root);

RecordMeta buildRecordMeta(const RecordConfig& config, bool addId = false);

}  // namespace crystal
