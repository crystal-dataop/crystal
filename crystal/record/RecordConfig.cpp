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

#include "crystal/foundation/Logging.h"
#include "crystal/record/RecordConfig.h"
#include "crystal/record/containers/String.h"
#include "crystal/record/containers/TupleMetaGenerator.h"

namespace crystal {

bool FieldConfig::parse(const dynamic& root) {
  auto name = root.getDefault("name");
  if (name.empty()) {
    CRYSTAL_LOG(ERROR) << "miss name: " << toCson(root);
    return false;
  }
  name_ = name.getString();
  auto type = root.getDefault("type");
  if (type.empty()) {
    auto related = root.getDefault("related");
    if (related.empty()) {
      CRYSTAL_LOG(ERROR) << "miss type or related: " << toCson(root);
      return false;
    }
    related_ = related.getString();
    type_ = DataType::UINT64;
  } else {
    DataType dtype = stringToDataType(type.c_str());
    if (dtype == DataType::UNKNOWN) {
      CRYSTAL_LOG(ERROR) << "unknown type: " << type.getString()
        << ", " << toCson(root);
      return false;
    }
    if (dtype == DataType::TUPLE) {
      auto meta = root.getDefault("meta");
      if (meta.empty()) {
        CRYSTAL_LOG(ERROR) << "miss tuple meta: " << toCson(root);
        return false;
      }
      for (auto& field : meta) {
        FieldConfig config;
        if (!config.parse(field)) {
          CRYSTAL_LOG(ERROR) << "parse tuple field faild: " << toCson(field);
          return false;
        }
        tuple_.push_back(std::move(config));
      }
    } else {
      default_ = root.getDefault("default", nullptr);
    }
    type_ = dtype;
    auto count = root.getDefault("count", -1);
    if (count >= 0) {
      count_ = count.getInt();
    }
  }
  return true;
}

const std::string& FieldConfig::name() const {
  return name_;
}

DataType FieldConfig::type() const {
  return type_;
}

size_t FieldConfig::count() const {
  return count_;
}

const dynamic& FieldConfig::dflt() const {
  return default_;
}

const std::vector<FieldConfig>& FieldConfig::tuple() const {
  return tuple_;
}

const std::string& FieldConfig::related() const {
  return related_;
}

bool FieldConfig::isRelated() const {
  return !related_.empty();
}

dynamic FieldConfig::toDynamic() const {
  dynamic j = dynamic::object("name", name_);
  if (!related_.empty()) {
    j.insert("related", related_);
  } else {
    j.insert("type", dataTypeToString(type_));
    if (count_ != 1) {
      j.insert("count", count_);
    }
    if (type_ == DataType::TUPLE) {
      dynamic a = dynamic::array;
      for (auto& field : tuple_) {
        a.push_back(field.toDynamic());
      }
      j.insert("meta", a);
    } else {
      if (!default_.isNull()) {
        j.insert("default", default_);
      }
    }
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
    recordConfig.fieldIndex[config.name()] = recordConfig.fieldConfigs.size();
    recordConfig.fieldConfigs.push_back(std::move(config));
  }
  return recordConfig;
}

untyped_tuple::meta buildTupleMeta(const std::vector<FieldConfig>& configs) {
  untyped_tuple::meta meta;
  meta.reserve(configs.size());
  for (auto& field : configs) {
    switch (field.type()) {
#define CASE(dt, t)                                             \
      case DataType::dt:                                        \
        meta.add_type<t>(untyped_tuple::meta{}, field.count()); \
        break;

      CASE(BOOL, bool)
      CASE(INT8, int8_t)
      CASE(INT16, int16_t)
      CASE(INT32, int32_t)
      CASE(INT64, int64_t)
      CASE(UINT8, uint8_t)
      CASE(UINT16, uint16_t)
      CASE(UINT32, uint32_t)
      CASE(UINT64, uint64_t)
      CASE(FLOAT, float)
      CASE(DOUBLE, double)
      CASE(STRING, string)

      case DataType::TUPLE:
        meta.add_type<untyped_tuple>(buildTupleMeta(field.tuple()),
                                     field.count());
        break;
      default: break;

#undef CASE
    }
  }
  Singleton<AtomicLinkedList<TupleMetaWrapper>>::get()
    .insertHead(TupleMetaWrapper{meta});
  return meta;
}

std::vector<FieldConfig> RecordConfig::collect(
    const std::string& fields, bool addId) {
  std::vector<FieldConfig> configs;
  if (fields == "*") {
    configs = fieldConfigs;
  } else {
    std::vector<std::string> fieldVec;
    split(',', fields, fieldVec);
    for (auto& field : fieldVec) {
      auto it = fieldIndex.find(field);
      if (it != fieldIndex.end()) {
        configs.push_back(fieldConfigs[it->second]);
      }
    }
  }
  if (addId) {
    FieldConfig config;
    config.parse(dynamic::object("name", "__id")("type", "uint64"));
    configs.insert(configs.begin(), config);
  }
  return configs;
}

untyped_tuple::meta RecordConfig::buildRecordMeta(
    const std::string& fields, bool addId) {
  return buildTupleMeta(collect(fields, addId));
}

} // namespace crystal
