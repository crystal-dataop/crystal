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

#include "crystal/storage/builder/IdUtil.h"
#include "crystal/storage/builder/RecordBuilder.h"
#include "crystal/storage/table/Table.h"

namespace crystal {

class TableBuilder {
 public:
  TableBuilder(Table* table);
  virtual ~TableBuilder() {}

  Table* table() const;

  uint64_t getId(const dynamic& j);

  bool add(const dynamic& j);
  bool addOnKV(const dynamic& j);
  bool addOnIndex(const std::string& name, const dynamic& j);

  bool update(const dynamic& j);
  bool updateOnKV(const dynamic& j);
  bool updateOnIndex(const std::string& name, const dynamic& j);

  bool remove(const dynamic& j);
  bool removeOnKV(const dynamic& j);
  bool removeOnIndex(const std::string& name, const dynamic& j);

 private:
  struct KVKeyId {
    KV* kv;
    uint64_t key;
    uint32_t id;

    KVKeyId(KV* p, uint64_t k, uint32_t i)
        : kv(p), key(k), id(i) {}
  };

  KVKeyId getKVKeyId(const Record& record, uint32_t id = -1) const;

  std::vector<std::pair<Index*, uint64_t>>
    getIndex(const std::string& name, const Record& record) const;

  Table* table_;
  RecordBuilder<SysAllocator> recordBuilder_;
  IdGenerator idGenerator_;
};

} // namespace crystal
