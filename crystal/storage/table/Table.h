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

#include <bitset>
#include <map>
#include <string>
#include <vector>

#include "crystal/storage/index/Index.h"
#include "crystal/storage/kv/KV.h"
#include "crystal/storage/table/TableConfig.h"

namespace crystal {

typedef std::bitset<1024> IndexNoSet;

class Table {
 public:
  explicit Table(const TableConfig& config);
  virtual ~Table() {}

  Table(const Table&) = delete;
  Table& operator=(const Table&) = delete;

  bool init(const char* path, bool readOnly);

  void dump();

  const TableConfig& config() const;
  const RecordMeta& recordMeta() const;

  bool fieldInKV(const std::string& field) const;
  bool fieldInIndex(const std::string& field, const IndexNoSet* set) const;

  size_t getNoOfIndex(const std::string& index) const;

  /*
   * KV
   */

  size_t getKVSegmentCount() const;
  KV* getKV(uint16_t seg) const;
  KV* getKVById(uint64_t id) const;
  KV* getKVByKey(uint64_t key) const;

  uint64_t find(uint64_t key) const;
  bool insert(uint64_t key, uint64_t id);
  bool erase(uint64_t key);

  /*
   * Index
   */

  size_t getIndexSegmentCount(const std::string& index) const;
  Index* getIndex(const std::string& index, uint16_t seg) const;
  Index* getIndexByToken(const std::string& index, uint64_t token) const;

  AnyPostingList getPostingList(const std::string& index, uint64_t token) const;

 private:
  bool initKV();
  bool initIndex();

  TableConfig config_;
  RecordMeta recordMeta_;

  std::set<std::string> kvFields_;
  std::map<std::string, std::pair<bool, IndexNoSet>> indexFields_;
  std::vector<std::string> indexes_;

  std::string path_;
  bool readOnly_;
  std::vector<std::unique_ptr<MemoryManager>> memorys_;
  std::vector<std::unique_ptr<KV>> kvs_;
  std::map<std::string, std::vector<std::unique_ptr<Index>>> indexMap_;
};

//////////////////////////////////////////////////////////////////////

inline const TableConfig& Table::config() const {
  return config_;
}

inline const RecordMeta& Table::recordMeta() const {
  return recordMeta_;
}

inline bool Table::fieldInKV(const std::string& field) const {
  return kvFields_.find(field) != kvFields_.end();
}

inline bool Table::fieldInIndex(
    const std::string& field, const IndexNoSet* set) const {
  auto it = indexFields_.find(field);
  if (it == indexFields_.end()) {
    return false;
  }
  set = &it->second.second;
  return true;
}

inline size_t Table::getKVSegmentCount() const {
  return kvs_.size();
}

inline KV* Table::getKV(uint16_t seg) const {
  return seg < kvs_.size() ? kvs_[seg].get() : nullptr;
}

inline KV* Table::getKVById(uint64_t id) const {
  return getKV(id >> 32);
}

inline KV* Table::getKVByKey(uint64_t key) const {
  return !kvs_.empty() ? getKV(key % kvs_.size()) : nullptr;
}

}  // namespace crystal
