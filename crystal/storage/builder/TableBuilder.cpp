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

#include "crystal/storage/builder/TableBuilder.h"

#include <stdexcept>

#include "crystal/foundation/ScopeGuard.h"
#include "crystal/strategy/DefaultStrategy.h"

namespace crystal {

TableBuilder::TableBuilder(Table* table)
    : table_(table),
      recordBuilder_(table->config().recordConfig(), true) {
  recordBuilder_.init(nullptr);
}

TableBuilder::KVKeyId
TableBuilder::getKVKeyId(const Record& record, uint32_t id) const {
  auto meta = table_->config().kvConfig().keyConfig().toFieldMeta();
  if (!record.hasField(meta)) {
    throw std::runtime_error(
        "miss key field: " + table_->config().kvConfig().key());
  }
  DefaultStrategy strategy(meta);
  auto keys = strategy.getIndexKeys(record);
  if (keys.size() != 1) {
    throw std::runtime_error("get hashed key failed");
  }
  uint64_t key = keys[0];
  KV* kv = table_->getKVByKey(key);
  if (id != uint32_t(-1)) {
    if (!kv->insert(key, id)) {
      throw std::runtime_error(toString("insert key=", key, " failed"));
    }
  } else {
    id = kv->find(key);
    if (id == uint32_t(-1)) {
      throw std::runtime_error(toString("key=", key, " not found"));
    }
  }
  return { kv, key, id };
}

std::vector<std::pair<Index*, uint64_t>>
TableBuilder::getIndex(const std::string& name, const Record& record) const {
  auto& config = table_->config().indexConfig(name);
  auto meta = config.keyConfig().toFieldMeta();
  if (!record.hasField(meta)) {
    throw std::runtime_error("miss key field: " + config.key());
  }
  auto strategy = createStrategy(config.strategy(), meta);
  auto keys = strategy->getIndexKeys(record);
  if (keys.empty()) {
    throw std::runtime_error("get hashed key failed");
  }
  std::vector<std::pair<Index*, uint64_t>> indexes;
  for (uint64_t key : keys) {
    indexes.push_back(std::make_pair(table_->getIndexByToken(name, key), key));
  }
  return indexes;
}

Table* TableBuilder::table() const {
  return table_;
}

uint64_t TableBuilder::getId(const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  auto meta = table_->config().kvConfig().keyConfig().toFieldMeta();
  if (!record.hasField(meta)) {
    throw std::runtime_error(
        "miss key field: " + table_->config().kvConfig().key());
  }
  DefaultStrategy strategy(meta);
  auto keys = strategy.getIndexKeys(record);
  if (keys.size() != 1) {
    throw std::runtime_error("get hashed key failed");
  }
  uint64_t key = keys[0];
  return table_->find(key);
}

bool TableBuilder::addOnKV(const dynamic& j) {
  uint32_t id = idGenerator_.generate();
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record, id);
    if (!p.kv->add(p.id, record)) {
      CRYSTAL_LOG(ERROR) << "add record failed";
      return false;
    }
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  return true;
}

bool TableBuilder::addOnIndex(const std::string& name, const dynamic& j) {
  uint32_t id = idGenerator_.generate();
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  record.set<uint64_t>("__id", id);
  try {
    auto indexes = getIndex(name, record);
    for (auto& index : indexes) {
      if (!index.first->add(index.second, record)) {
        CRYSTAL_LOG(ERROR) << "add record failed";
        return false;
      }
    }
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get index failed: " << e.what();
    return false;
  }
  return true;
}

bool TableBuilder::add(const dynamic& j) {
  uint32_t id = idGenerator_.generate();
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record, id);
    if (!p.kv->add(p.id, record)) {
      CRYSTAL_LOG(WARN) << "add record failed";
    }
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(DEBUG) << "get kvkeyid failed: " << e.what();
  }
  record.set<uint64_t>("__id", id);
  for (auto& config : table_->config().indexConfigs()) {
    try {
      auto indexes = getIndex(config.first, record);
      for (auto& index : indexes) {
        if (!index.first->add(index.second, record)) {
          CRYSTAL_LOG(WARN) << "add record failed";
        }
      }
    } catch (const std::runtime_error& e) {
      CRYSTAL_LOG(DEBUG) << "get index failed: " << e.what();
    }
  }
  return true;
}

bool TableBuilder::updateOnKV(const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record);
    if (!p.kv->update(p.id, record)) {
      CRYSTAL_LOG(ERROR) << "update record failed";
      return false;
    }
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  return true;
}

bool TableBuilder::updateOnIndex(
    const std::string& name, const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record);
    record.set<uint64_t>("__id", p.id);
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  try {
    auto indexes = getIndex(name, record);
    for (auto& index : indexes) {
      if (!index.first->update(index.second, record)) {
        CRYSTAL_LOG(ERROR) << "update record failed";
        return false;
      }
    }
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get index failed: " << e.what();
    return false;
  }
  return true;
}

bool TableBuilder::update(const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record);
    if (!p.kv->update(p.id, record)) {
      CRYSTAL_LOG(WARN) << "update record failed";
    }
    record.set<uint64_t>("__id", p.id);
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  for (auto& config : table_->config().indexConfigs()) {
    try {
      auto indexes = getIndex(config.first, record);
      for (auto& index : indexes) {
        if (!index.first->update(index.second, record)) {
          CRYSTAL_LOG(WARN) << "update record failed";
        }
      }
    } catch (const std::runtime_error& e) {
      CRYSTAL_LOG(DEBUG) << "get index failed: " << e.what();
    }
  }
  return true;
}

bool TableBuilder::removeOnKV(const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record);
    if (!p.kv->remove(p.id)) {
      CRYSTAL_LOG(ERROR) << "remove record failed";
      return false;
    }
    p.kv->erase(p.key);
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  return true;
}

bool TableBuilder::removeOnIndex(
    const std::string& name, const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record);
    record.set<uint64_t>("__id", p.id);
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  try {
    auto indexes = getIndex(name, record);
    for (auto& index : indexes) {
      if (!index.first->remove(index.second, record)) {
        CRYSTAL_LOG(ERROR) << "remove record failed";
        return false;
      }
    }
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get index failed: " << e.what();
    return false;
  }
  return true;
}

bool TableBuilder::remove(const dynamic& j) {
  Record record = recordBuilder_.build(j);
  CRYSTAL_SCOPE_EXIT {
    recordBuilder_.release();
  };
  try {
    auto p = getKVKeyId(record);
    if (!p.kv->remove(p.id)) {
      CRYSTAL_LOG(WARN) << "remove record failed";
    } else {
      p.kv->erase(p.key);
    }
    record.set<uint64_t>("__id", p.id);
  } catch (const std::runtime_error& e) {
    CRYSTAL_LOG(ERROR) << "get kvkeyid failed: " << e.what();
    return false;
  }
  for (auto& config : table_->config().indexConfigs()) {
    try {
      auto indexes = getIndex(config.first, record);
      for (auto& index : indexes) {
        if (!index.first->remove(index.second, record)) {
          CRYSTAL_LOG(WARN) << "remove record failed";
        }
      }
    } catch (const std::runtime_error& e) {
      CRYSTAL_LOG(DEBUG) << "get index failed: " << e.what();
    }
  }
  return true;
}

} // namespace crystal
