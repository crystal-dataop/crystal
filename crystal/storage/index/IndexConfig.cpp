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

#include "crystal/storage/index/IndexConfig.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

bool IndexConfig::parse(const dynamic& root, const RecordConfig& recordConfig) {
  if (!KVConfig::parse(root, recordConfig, "payload", true)) {
    return false;
  }
  auto type = root.getDefault("type");
  if (type.empty()) {
    CRYSTAL_LOG(ERROR) << "miss type: " << toCson(root);
    return false;
  }
  type_ = type.getString();
  if (strcasecmp(type_.substr(0, 5).c_str(), "Faiss") == 0) {
    vectorMeta_.type = VectorType::Faiss;
    if (type_.size() > 5) {
      vectorMeta_.desc = type_.substr(strlen("Faiss:"));
      type_ = "Faiss";
    }
    auto dimension = root.getDefault("dimension", -1);
    if (dimension.getInt() < 0) {
      CRYSTAL_LOG(ERROR) << "miss dimension: " << toCson(root);
      return false;
    }
    vectorMeta_.dimension = dimension.getInt();
    auto metric = root.getDefault("metric");
    if (metric.empty()) {
      CRYSTAL_LOG(ERROR) << "miss metric: " << toCson(root);
      return false;
    }
    vectorMeta_.metric = stringToFaissMetric(metric.asString().c_str());
    vectorMeta_.trainSize = root.getDefault("trainSize",
                                            vectorMeta_.trainSize).getInt();
    vectorMeta_.batchSize = root.getDefault("batchSize",
                                            vectorMeta_.batchSize).getInt();
  }
  return true;
}

const std::string& IndexConfig::type() const {
  return type_;
}

const VectorMeta& IndexConfig::vectorMeta() const {
  return vectorMeta_;
}

}  // namespace crystal
