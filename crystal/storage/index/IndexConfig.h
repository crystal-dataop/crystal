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

#include <string>
#include <vector>

#include "crystal/storage/kv/KVConfig.h"
#include "crystal/storage/index/vector/VectorMeta.h"

namespace crystal {

class IndexConfig : public KVConfig {
 public:
  IndexConfig() : KVConfig() {}

  bool parse(const dynamic& root, const RecordConfig& recordConfig) override;

  const std::string& type() const;
  const VectorMeta& vectorMeta() const;

 private:
  std::string type_;
  VectorMeta vectorMeta_;
};

}  // namespace crystal
