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

#include "crystal/serializer/record/RecordConfig.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

bool FieldConfig::parse(const dynamic& root) {
  auto name = root.getDefault("name");
  if (name.empty()) {
    CRYSTAL_LOG(ERROR) << "miss name: " << toCson(root);
    return false;
  }
  name_ = name.getString();
  auto tag = root.getDefault("tag", -1);
  if (tag < 0) {
    CRYSTAL_LOG(ERROR) << "miss tag: " << toCson(root);
    return false;
  }
  tag_ = tag.getInt();
  auto type = root.getDefault("type");
  if (type.empty()) {
    CRYSTAL_LOG(ERROR) << "miss type: " << toCson(root);
    return false;
  }
  if (type.getString() == "related") {
    auto table = root.getDefault("table");
    if (table.empty()) {
      CRYSTAL_LOG(ERROR) << "miss table: " << toCson(root);
      return false;
    }
    related_ = table.getString();
    type_ = DataType::UINT64;
  } else {
    DataType dtype = stringToDataType(type.c_str());
    if (dtype == DataType::UNKNOWN) {
      CRYSTAL_LOG(ERROR) << "unknown type: " << type.getString()
        << ", " << toCson(root);
      return false;
    }
    type_ = dtype;
  }
  if (isArithmetic(type_)) {
    bits_ = sizeOf(type_) * 8;
  }
  if (!isRelated()) {
    auto bits = root.getDefault("bits", -1);
    if (bits > 0 && isIntegral(type_)) {
      bits_ = bits.getInt();
    }
    auto count = root.getDefault("count", -1);
    if (count >= 0) {
      count_ = count.getInt();
    }
    default_ = root.getDefault("default", nullptr);
  }
  return true;
}

const std::string& FieldConfig::name() const {
  return name_;
}

int FieldConfig::tag() const {
  return tag_;
}

DataType FieldConfig::type() const {
  return type_;
}

size_t FieldConfig::bits() const {
  return bits_;
}

size_t FieldConfig::count() const {
  return count_;
}

const dynamic& FieldConfig::dflt() const {
  return default_;
}

const std::string& FieldConfig::related() const {
  return related_;
}

bool FieldConfig::isRelated() const {
  return !related_.empty();
}

FieldMeta FieldConfig::toFieldMeta() const {
  return FieldMeta(name_, tag_, type_, bits_, count_, default_);
}

dynamic FieldConfig::toDynamic() const {
  dynamic j = dynamic::object
    ("name", name_)
    ("tag", tag_)
    ("type", dataTypeToString(type_))
    ("bits", bits_)
    ("count", count_);
  if (!default_.isNull()) {
    j.insert("default", default_);
  }
  if (!related_.empty()) {
    j.insert("type", "related");
    j.insert("table", related_);
  }
  return j;
}

std::string FieldConfig::toString() const {
  return toCson(toDynamic());
}

RecordConfig parseRecordConfig(const dynamic& root) {
  auto record = root.getDefault("record");
  if (record.empty()) {
    CRYSTAL_LOG(ERROR) << "miss record: " << toCson(root);
    return RecordConfig();
  }
  RecordConfig recordConfig;
  for (auto& field : record) {
    FieldConfig config;
    if (!config.parse(field)) {
      CRYSTAL_LOG(ERROR) << "parse failed: " << toCson(field);
      return RecordConfig();
    }
    recordConfig[config.name()] = config;
  }
  return recordConfig;
}

RecordMeta buildRecordMeta(const RecordConfig& config, bool addId) {
  RecordMeta recordMeta;
  for (auto& kv : config) {
    if (!recordMeta.addMeta(kv.second.toFieldMeta())) {
      CRYSTAL_LOG(ERROR) << "add field meta failed";
    }
  }
  if (addId) {
    FieldMeta idMeta("__id", recordMeta.maxTag() + 1, DataType::UINT64, 64, 1);
    if (!recordMeta.addMeta(idMeta)) {
      CRYSTAL_LOG(ERROR) << "add id meta failed";
    }
  }
  return recordMeta;
}

}  // namespace crystal
