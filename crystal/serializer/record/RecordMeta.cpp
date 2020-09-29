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

#include "crystal/serializer/record/RecordMeta.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

void RecordMeta::clear() {
  metas_.clear();
  nameMap_.clear();
  tagMap_.clear();
  maxTag_ = 0;
}

bool RecordMeta::addMeta(const FieldMeta& meta) {
  auto& name = meta.name();
  if (nameMap_.find(name) != nameMap_.end()) {
    CRYSTAL_LOG(ERROR) << "duplicate field name: " << name;
    return false;
  }
  int tag = meta.tag();
  if (tagMap_.find(tag) != tagMap_.end()) {
    CRYSTAL_LOG(ERROR) << "duplicate field tag: " << tag;
    return false;
  }
  nameMap_[name] = metas_.size();
  tagMap_[tag] = metas_.size();
  metas_.push_back(meta);
  maxTag_ = std::max(tag, maxTag_);
  return true;
}

dynamic RecordMeta::toDynamic() const {
  dynamic j = dynamic::array;
  for (auto& meta : metas_) {
    j.push_back(meta.toDynamic());
  }
  return j;
}

std::string RecordMeta::toString() const {
  return toCson(toDynamic());
}

}  // namespace crystal
