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

#include <vector>

#include "crystal/type/Utility.h"

namespace crystal {

struct FieldIndex {
  std::unordered_map<std::string, size_t> index;

  size_t size() const;

  void build(const std::vector<std::string>& fields);
  void append(const std::string& field);

  size_t getIndexOfField(const std::string& field) const;

  void select(std::vector<std::string>& fields);

  bool merge(FieldIndex& other);

 private:
  size_t fieldCount_{0};
};

//////////////////////////////////////////////////////////////////////

inline size_t FieldIndex::size() const {
  return index.size();
}

inline size_t FieldIndex::getIndexOfField(const std::string& field) const {
  auto it = index.find(field);
  return it != index.end() ? it->second : npos;
}

} // namespace crystal
