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

#include "crystal/graph/Graph.h"
#include "crystal/storage/table/TableFactory.h"

namespace crystal {

class DataView;

class Query {
 public:
  Query(TableFactory* factory, Graph::Executor* executor, bool useCson = true);

  virtual ~Query() {}

  Query& operator+=(const std::string& str) {
    query_ += str;
    return *this;
  }

  const std::string& query() const {
    return query_;
  }

  DataView run();

  std::string runAndToJson(bool tableMode, bool prettify);
  std::string runAndToCson(bool tableMode, bool prettify);

 private:
  TableFactory* factory_;
  Graph graph_;
  bool useCson_;
  std::string query_;
};

}  // namespace crystal
