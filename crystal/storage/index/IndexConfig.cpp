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
  return true;
}

const std::string& IndexConfig::type() const {
  return type_;
}

}  // namespace crystal
