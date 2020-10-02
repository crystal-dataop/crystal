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

#include <functional>
#include <map>
#include <variant>

#include "crystal/dataframe/DataView.h"

namespace crystal {

struct OpContext {
  DataView* view;
  dynamic param;
};

typedef std::function<void(OpContext&)> QueryOp;
typedef std::function<int(OpContext&)> ConditionQueryOp;

typedef std::variant<QueryOp, ConditionQueryOp> QueryOpVar;

class OpRegistry {
 public:
  static OpRegistry& getInstance();

  void add(const std::string& name, QueryOp&& op);
  void add(const std::string& name, ConditionQueryOp&& op);

  const QueryOpVar* get(const std::string& name) const;

 private:
  OpRegistry() = default;
  ~OpRegistry() = default;

  std::map<std::string, QueryOpVar> opMap_;
};

template <class Op>
struct OpRegistryReceiver;

template <>
struct OpRegistryReceiver<QueryOp> {
  OpRegistryReceiver(const char* name, QueryOp op);
};

template <>
struct OpRegistryReceiver<ConditionQueryOp> {
  OpRegistryReceiver(const char* name, ConditionQueryOp op);
};

}  // namespace crystal
