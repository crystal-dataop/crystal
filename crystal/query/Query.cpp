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

#include "crystal/query/Query.h"

#include "crystal/graph/Graph.h"
#include "crystal/operator/generic/Serialize.h"

namespace crystal {

Query::Query(TableFactory* factory, Graph::Executor* executor, bool useCson)
    : factory_(factory), graph_(executor), useCson_(useCson) {
}

DataView Query::run() {
  DataView view;
  auto jq = useCson_ ? parseCson(query_) : parseJson(query_);
  auto path = jq["path"].asString();
  ExtendedTable* extable = factory_->getExtendedTable(path);
  if (!extable) {
    CRYSTAL_LOG(ERROR) << "table at path '" << path << "' not exist";
    return view;
  }
  view = DataView(std::make_unique<DocumentArray>(extable));
  graph_.gen(jq["graph"]);
  graph_.run(&view);
  return view;
}

std::string Query::runAndToJson(bool tableMode, bool prettify) {
  DataView view = run();
  return view | op::toJson(tableMode, prettify);
}

std::string Query::runAndToCson(bool tableMode, bool prettify) {
  DataView view = run();
  return view | op::toCson(tableMode, prettify);
}

}  // namespace crystal
