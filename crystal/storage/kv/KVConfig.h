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

#include "crystal/serializer/record/RecordConfig.h"

namespace crystal {

class KVConfig {
 public:
  KVConfig(const std::string& name = "")
      : name_(name) {}

  virtual bool parse(const dynamic& root,
                     const RecordConfig& recordConfig);

  const std::string& name() const;
  const std::string& key() const;
  const FieldConfig& keyConfig() const;
  const RecordConfig& fields() const;
  size_t bucket() const;

 protected:
  bool parse(const dynamic& root,
             const RecordConfig& recordConfig,
             const std::string& valueName,
             bool valueIsOptional);

 private:
  static constexpr size_t kBucketSize = 1ul << 24;

  std::string name_;
  std::string key_;
  FieldConfig keyConfig_;
  RecordConfig fields_;
  size_t bucket_{kBucketSize};
};

}  // namespace crystal
