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

#include "crystal/storage/table/FieldIndex.h"

namespace crystal {

void FieldIndex::build(const std::vector<std::string>& fields) {
  index.clear();
  size_t i = 0;
  for (auto field : fields) {
    index.emplace(field, i);
    ++i;
  }
  fieldCount_ = i;
}

void FieldIndex::append(const std::string& field) {
  index.emplace(field, fieldCount_);
  ++fieldCount_;
}

void FieldIndex::select(std::vector<std::string>& fields) {
  std::unordered_map<std::string, size_t> selected;
  std::vector<std::string> miss;
  for (auto field : fields) {
    auto it = index.find(field);
    if (it != index.end()) {
      selected.insert(*it);
    } else {
      miss.push_back(field);
    }
  }
  index.swap(selected);
  fields.swap(miss);
}

bool FieldIndex::merge(FieldIndex& other) {
  return index == other.index;
}

} // namespace crystal
