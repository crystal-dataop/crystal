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

#include "crystal/graph/OpRegistry.h"

namespace crystal {

OpRegistry& OpRegistry::getInstance() {
  static OpRegistry registry;
  return registry;
}

void OpRegistry::add(const std::string& name, QueryOp&& op) {
  opMap_.emplace(name, std::move(op));
}

void OpRegistry::add(const std::string& name, ConditionQueryOp&& op) {
  opMap_.emplace(name, std::move(op));
}

const QueryOpVar* OpRegistry::get(const std::string& name) const {
  auto it = opMap_.find(name);
  return it == opMap_.end() ? nullptr : &it->second;
}

OpRegistryReceiver<QueryOp>::OpRegistryReceiver(
    const char* name, QueryOp op) {
  OpRegistry::getInstance().add(name, std::move(op));
}

OpRegistryReceiver<ConditionQueryOp>::OpRegistryReceiver(
    const char* name, ConditionQueryOp op) {
  OpRegistry::getInstance().add(name, std::move(op));
}

}  // namespace crystal
