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
#include <unordered_map>
#include <vector>

#include "crystal/foundation/json.h"
#include "crystal/serializer/record/FieldMeta.h"

namespace crystal {

class RecordMeta {
 public:
  typedef std::vector<FieldMeta>::iterator iterator;
  typedef std::vector<FieldMeta>::const_iterator const_iterator;

  RecordMeta() {}
  virtual ~RecordMeta() {}

  void clear();

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bool addMeta(const FieldMeta& meta);

  const FieldMeta* getMeta(const std::string& name) const;
  const FieldMeta* getMeta(int tag) const;

  int maxTag() const;

  dynamic toDynamic() const;
  std::string toString() const;

 private:
  std::vector<FieldMeta> metas_;
  std::unordered_map<std::string, size_t> nameMap_;
  std::unordered_map<int, size_t> tagMap_;
  int maxTag_{0};
};

//////////////////////////////////////////////////////////////////////

inline RecordMeta::iterator RecordMeta::begin() {
  return metas_.begin();
}

inline RecordMeta::const_iterator RecordMeta::begin() const {
  return metas_.begin();
}

inline RecordMeta::iterator RecordMeta::end() {
  return metas_.end();
}

inline RecordMeta::const_iterator RecordMeta::end() const {
  return metas_.end();
}

inline const FieldMeta* RecordMeta::getMeta(const std::string& name) const {
  auto it  = nameMap_.find(name);
  return it != nameMap_.end() ? &metas_[it->second] : nullptr;
}

inline const FieldMeta* RecordMeta::getMeta(int tag) const {
  auto it  = tagMap_.find(tag);
  return it != tagMap_.end() ? &metas_[it->second] : nullptr;
}

inline int RecordMeta::maxTag() const {
  return maxTag_;
}

}  // namespace crystal
