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

#include "crystal/storage/kv/KV.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

KV::KV(const KVConfig& config)
    : config_(&config),
      recordMeta_(buildRecordMeta(config.fields())),
      keyMeta_(config.keyConfig().toFieldMeta()),
      accessor_(recordMeta_),
      keyIdMap_(config.bucket()),
      chunkMap_(accessor_.bufferSize()) {
}

bool KV::init(MemoryManager* memory) {
  if (!alloc_.init(memory->getMemory(MemoryType::kMemRecyc))) {
    CRYSTAL_LOG(ERROR) << "init ml_allocator failed";
    return false;
  }
  if (!keyIdMap_.init(memory->getMemory(MemoryType::kMemHash))) {
    CRYSTAL_LOG(ERROR) << "init key-id map failed";
    return false;
  }
  if (!chunkMap_.init(memory->getMemory(MemoryType::kMemSimple))) {
    CRYSTAL_LOG(ERROR) << "init chunk map failed";
    return false;
  }
  if (!bitMaskMap_.init(memory->getMemory(MemoryType::kMemBit))) {
    CRYSTAL_LOG(ERROR) << "init bitmask map failed";
    return false;
  }
  return true;
}

bool KV::insert(uint64_t key, uint32_t id) {
  auto p = keyIdMap_.emplace(key, std::forward<uint32_t>(id));
  if (!p.second) {
    p.first->second.data = id;
  }
  if (!bitMaskMap_.set(id)) {
    CRYSTAL_LOG(ERROR) << "set bitmask map failed, id=" << id;
    return false;
  }
  return true;
}

void KV::erase(uint64_t key) {
  auto it = keyIdMap_.find(key);
  if (it != keyIdMap_.cend()) {
    it->second.data = -1;
  }
}

bool KV::add(uint32_t id, const Record& newRecord) {
  if (exist(id)) {
    CRYSTAL_LOG(ERROR) << "record id=" << id << " already exist";
    return false;
  }
  if (id >= chunkMap_.size()) {
    if (!chunkMap_.expand(id + 1)) {
      CRYSTAL_LOG(ERROR) << "expand chunk map size to " << id + 1 << " failed";
      return false;
    }
  }
  Record record = createRecord();
  record.setBuffer(getRecordPtr(id));
  if (!record.copy(newRecord)) {
    CRYSTAL_LOG(ERROR) << "copy from newRecord failed, id=" << id;
    return false;
  }
  if (!bitMaskMap_.unset(id)) {
    CRYSTAL_LOG(ERROR) << "unset bitmask map failed, id=" << id;
    return false;
  }
  return true;
}

bool KV::update(uint32_t id, const Record& newRecord) {
  if (!exist(id)) {
    CRYSTAL_LOG(ERROR) << "record id=" << id << " not exist";
    return false;
  }
  if (!bitMaskMap_.set(id)) {
    CRYSTAL_LOG(ERROR) << "set bitmask map failed, id=" << id;
    return false;
  }
  Record record = createRecord();
  record.setBuffer(getRecordPtr(id));
  if (!record.merge(newRecord)) {
    CRYSTAL_LOG(ERROR) << "merge from newRecord failed, id=" << id;
    return false;
  }
  if (!bitMaskMap_.unset(id)) {
    CRYSTAL_LOG(ERROR) << "unset bitmask map failed, id=" << id;
    return false;
  }
  return true;
}

bool KV::remove(uint32_t id) {
  if (!exist(id)) {
    return true;
  }
  if (!bitMaskMap_.set(id)) {
    CRYSTAL_LOG(ERROR) << "set bitmask map failed, id=" << id;
    return false;
  }
  Record record = createRecord();
  record.setBuffer(getRecordPtr(id));
  if (!record.reset()) {
    CRYSTAL_LOG(ERROR) << "reset record failed, id=" << id;
    return false;
  }
  return true;
}

dynamic KV::serialize() {
  dynamic j = dynamic::object;
  auto it = keyIdMap_.cbegin();
  while (it != keyIdMap_.cend()) {
    Record record = createRecord();
    getUnsafe(it->second.data, record);
    j[it->first] = dynamic::object
        ("id", it->second.data)
        ("record", record.toDynamic());
    ++it;
  }
  return j;
}

}  // namespace crystal
