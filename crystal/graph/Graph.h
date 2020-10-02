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

#include "crystal/graph/OpRegistry.h"
#include "crystal/graph/taskflow/executor.hpp"

namespace crystal {

class Graph {
 public:
  typedef tf::Executor Executor;

  Graph(Executor* executor) : executor_(executor) {}

  bool gen(const dynamic& graph);
  bool genPipeline(const dynamic& pipeline);

  void run(DataView* view);

 private:
  Executor* executor_;
  tf::Taskflow taskflow_;
  std::unordered_map<std::string, OpContext> contexts_;
  std::unordered_map<std::string, tf::Task> tasks_;
};

}  // namespace crystal
