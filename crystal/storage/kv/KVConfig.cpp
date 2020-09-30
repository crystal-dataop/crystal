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

#include "crystal/storage/kv/KVConfig.h"

#include <vector>

#include "crystal/foundation/Logging.h"

namespace crystal {

bool KVConfig::parse(
    const dynamic& root,
    const RecordConfig& recordConfig,
    const std::string& valueName,
    bool valueIsOptional) {
  auto key = root.getDefault("key");
  if (key.empty()) {
    CRYSTAL_LOG(ERROR) << "miss key: " << toCson(root);
    return false;
  }
  key_ = key.getString();
  auto it = recordConfig.find(key_);
  if (it == recordConfig.end()) {
    CRYSTAL_LOG(ERROR) << "'" << key_ << "' not in record";
    return false;
  }
  keyConfig_ = it->second;
  DataType keyType = it->second.type();
  if (!isIntegral(keyType) && !isString(keyType)) {
    CRYSTAL_LOG(ERROR) << "unsupport key type: " << dataTypeToString(keyType);
    return false;
  }
  strategy_ = stringToStrategyType(
      root.getDefault("strategy", "default").getString().c_str());
  bucket_ = root.getDefault("bucket", kBucketSize).getInt();
  segment_ = root.getDefault("segment", 1).getInt();
  auto value = root.getDefault(valueName);
  if (value.empty()) {
    if (valueIsOptional) {
      return true;
    }
    CRYSTAL_LOG(ERROR) << "miss value: " << toCson(root);
    return false;
  }
  if (value == "*") {
    fields_ = recordConfig;
  } else {
    if (!value.isArray()) {
      CRYSTAL_LOG(ERROR) << "value should be array: " << toCson(root);
      return false;
    }
    for (auto& field : value) {
      auto f = field.getString();
      auto it = recordConfig.find(f);
      if (it == recordConfig.end()) {
        CRYSTAL_LOG(ERROR) << "'" << f << "' not in record";
        return false;
      }
      fields_[f] = it->second;
    }
  }
  return true;
}

bool KVConfig::parse(const dynamic& root, const RecordConfig& recordConfig) {
  return parse(root, recordConfig, "value", true);
}

const std::string& KVConfig::name() const {
  return name_;
}

const std::string& KVConfig::key() const {
  return key_;
}

const FieldConfig& KVConfig::keyConfig() const {
  return keyConfig_;
}

const RecordConfig& KVConfig::fields() const {
  return fields_;
}

StrategyType KVConfig::strategy() const {
  return strategy_;
}

size_t KVConfig::bucket() const {
  return bucket_;
}

uint16_t KVConfig::segment() const {
  return segment_;
}

}  // namespace crystal
