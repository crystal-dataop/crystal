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

#include "crystal/storage/index/IndexBase.h"

namespace crystal {

class Index {
 public:
  explicit Index(const IndexConfig& config);
  virtual ~Index() {}

  Index(const Index&) = delete;
  Index& operator=(const Index&) = delete;

  bool init(MemoryManager* memory);

  const IndexConfig& config() const;
  const RecordMeta& recordMeta() const;
  const FieldMeta& keyMeta() const;

  IndexBase* index() const;
  AnyPostingList getPostingList(uint64_t key);

  bool add(uint64_t key, const Record& record);
  bool update(uint64_t key, const Record& record);
  bool remove(uint64_t key, const Record& record);
  bool bulkLoad(uint64_t key, const std::vector<Record>& records);

 private:
  const IndexConfig* config_{nullptr};
  std::unique_ptr<IndexBase> index_;
};

//////////////////////////////////////////////////////////////////////

inline const IndexConfig& Index::config() const {
  return *config_;
}

inline const RecordMeta& Index::recordMeta() const {
  return index_->recordMeta();
}

inline const FieldMeta& Index::keyMeta() const {
  return index_->keyMeta();
}

inline IndexBase* Index::index() const {
  return index_.get();
}

inline AnyPostingList Index::getPostingList(uint64_t key) {
  return index_->getPostingList(key);
}

inline bool Index::add(uint64_t key, const Record& record) {
  return index_->add(key, record);
}

inline bool Index::update(uint64_t key, const Record& record) {
  return index_->update(key, record);
}

inline bool Index::remove(uint64_t key, const Record& record) {
  return index_->remove(key, record);
}

inline bool Index::bulkLoad(uint64_t key, const std::vector<Record>& records) {
  return index_->bulkLoad(key, records);
}

}  // namespace crystal
