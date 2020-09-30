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

#include "crystal/storage/builder/TableGroupBuilder.h"

#include "crystal/foundation/String.h"

namespace crystal {

TableGroupBuilder::TableGroupBuilder(TableGroup* tableGroup)
    : tableGroup_(tableGroup) {
  for (auto& kv : tableGroup_->getTables()) {
    builders_.emplace(kv.first,
                      std::make_unique<TableBuilder>(kv.second.get()));
  }
}

bool TableGroupBuilder::add(const std::string& path, const dynamic& j) {
  std::string name, type;
  split('.', path, name, type);
  auto it = builders_.find(name);
  if (it == builders_.end()) {
    CRYSTAL_LOG(ERROR) << "table '" << name << "' not found";
    return false;
  }
  auto copy = j;
  if (!resetRelated(copy, it->second->table()->config().relatedTables())) {
    CRYSTAL_LOG(ERROR) << "table '" << name << "' reset related fields failed";
    return false;
  }
  if (type == "*") {
    return it->second->add(copy);
  } else if (type == "_") {
    return it->second->addOnKV(copy);
  } else {
    return it->second->addOnIndex(type, copy);
  }
}

bool TableGroupBuilder::update(const std::string& path, const dynamic& j) {
  std::string name, type;
  split('.', path, name, type);
  auto it = builders_.find(name);
  if (it == builders_.end()) {
    CRYSTAL_LOG(ERROR) << "table '" << name << "' not found";
    return false;
  }
  if (type == "*") {
    return it->second->update(j);
  } else if (type == "_") {
    return it->second->updateOnKV(j);
  } else {
    return it->second->updateOnIndex(type, j);
  }
}

bool TableGroupBuilder::remove(const std::string& path, const dynamic& j) {
  std::string name, type;
  split('.', path, name, type);
  auto it = builders_.find(name);
  if (it == builders_.end()) {
    CRYSTAL_LOG(ERROR) << "table '" << name << "' not found";
    return false;
  }
  if (type == "*") {
    return it->second->remove(j);
  } else if (type == "_") {
    return it->second->removeOnKV(j);
  } else {
    return it->second->removeOnIndex(type, j);
  }
}

bool TableGroupBuilder::resetRelated(
    dynamic& j, const std::map<std::string, std::string>& related) const {
  for (auto& kv : related) {
    auto p = builders_.find(kv.second);
    if (p == builders_.end()) {
      CRYSTAL_LOG(ERROR) << "table '" << kv.second << "' not found";
      return false;
    }
    uint64_t id = p->second->getId(dynamic::object(kv.first, j[kv.first]));
    if (id == uint64_t(-1)) {
      CRYSTAL_LOG(ERROR) << kv.second << "__" << kv.first << " get id failed";
      return false;
    }
    j[kv.first] = id;
  }
  return true;
}

} // namespace crystal
