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

#include "crystal/memory/MemoryManager.h"
#include "crystal/memory/RecycledAllocator.h"
#include "crystal/serializer/record/Record.h"
#include "crystal/storage/kv/BitMaskMap.h"
#include "crystal/storage/kv/FixedChunkMap.h"
#include "crystal/storage/kv/HashMap.h"
#include "crystal/storage/kv/KVConfig.h"

namespace crystal {

class KV {
 public:
  explicit KV(const KVConfig& config);
  virtual ~KV() {}

  KV(const KV&) = delete;
  KV& operator=(const KV&) = delete;

  bool init(MemoryManager* memory);

  const KVConfig& config() const;
  const RecordMeta& recordMeta() const;
  const FieldMeta& keyMeta() const;

  /*
   * key -> id
   */

  uint32_t find(uint64_t key);
  bool insert(uint64_t key, uint32_t id);
  void erase(uint64_t key);

  /*
   * id -> record
   */

  bool exist(uint32_t id) const;

  Record createRecord(void* buf = nullptr) const;
  void* getRecordPtr(uint32_t id) const;
  bool get(uint32_t id, Record& record) const;
  bool getUnsafe(uint32_t id, Record& record) const;

  /*
   * modify
   */

  bool add(uint32_t id, const Record& newRecord);
  bool update(uint32_t id, const Record& newRecord);
  bool remove(uint32_t id);

  /*
   * serialize
   */

  dynamic serialize();

 private:
  const KVConfig* config_{nullptr};
  RecordMeta recordMeta_;
  FieldMeta keyMeta_;
  Accessor accessor_;
  mutable RecycledAllocator alloc_;
  HashMap<uint64_t, uint32_t> keyIdMap_;
  FixedChunkMap chunkMap_;
  BitMaskMap bitMaskMap_;
};

//////////////////////////////////////////////////////////////////////

inline const KVConfig& KV::config() const {
  return *config_;
}

inline const RecordMeta& KV::recordMeta() const {
  return recordMeta_;
}

inline const FieldMeta& KV::keyMeta() const {
  return keyMeta_;
}

inline uint32_t KV::find(uint64_t key) {
  auto it = keyIdMap_.find(key);
  return it != keyIdMap_.cend() ? it->second.data : -1;
}

inline bool KV::exist(uint32_t id) const {
  return id < chunkMap_.size() && !bitMaskMap_.isSet(id);
}

inline Record KV::createRecord(void* buf) const {
  return Record(&recordMeta_, &accessor_, &alloc_, buf);
}

inline void* KV::getRecordPtr(uint32_t id) const {
  return chunkMap_.getChunk(id);
}

inline bool KV::get(uint32_t id, Record& record) const {
  if (!exist(id)) {
    return false;
  }
  record.setBuffer(getRecordPtr(id));
  return true;
}

inline bool KV::getUnsafe(uint32_t id, Record& record) const {
  record.setBuffer(getRecordPtr(id));
  return true;
}

}  // namespace crystal
