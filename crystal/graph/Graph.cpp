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

#include "crystal/graph/Graph.h"

namespace crystal {

class QueryOpVarVisitor {
 public:
  QueryOpVarVisitor(OpContext* ctx)
      : ctx_(ctx) {}

  template <class T>
  void operator()(T& func) const {
    func(*ctx_);
  }

 private:
  OpContext* ctx_;
};

class QueryOpVarTfVisitor {
 public:
  QueryOpVarTfVisitor(tf::Taskflow* taskflow, OpContext* ctx)
      : taskflow_(taskflow), ctx_(ctx) {}

  template <class T>
  tf::Task operator()(T& func) const {
    return taskflow_->emplace([&]() { func(*ctx_); });
  }

 private:
  tf::Taskflow* taskflow_;
  OpContext* ctx_;
};

bool Graph::gen(const dynamic& graph) {
  for (auto& node : graph.items()) {
    auto nodeKey = node.first.asString();
    auto param = node.second[2];
    OpContext ctx;
    ctx.view = nullptr;
    ctx.param = param;
    contexts_.emplace(nodeKey, ctx);
  }
  for (auto& node : graph.items()) {
    auto nodeKey = node.first.asString();
    auto nodeOp = node.second[0].asString();
    auto func = OpRegistry::getInstance().get(nodeOp);
    auto& ctx = contexts_[nodeKey];
    if (!func) {
      CRYSTAL_LOG(ERROR) << nodeOp << " op not registered";
      return false;
    } else {
      QueryOpVarTfVisitor visitor(&taskflow_, &ctx);
      tasks_.emplace(nodeKey, std::visit(visitor, *func).name(nodeKey));
    }
  }
  for (auto& node : graph.items()) {
    auto nodeKey = node.first.asString();
    auto nodeNext = node.second[1];
    if (!nodeNext.isNull()) {
      if (nodeNext.isArray()) {
        for (auto& next : nodeNext) {
          tasks_[nodeKey].precede(tasks_[next.asString()]);
        }
      } else {
        tasks_[nodeKey].precede(tasks_[nodeNext.asString()]);
      }
    }
  }
  return true;
}

bool Graph::genPipeline(const dynamic& pipeline) {
  for (auto& node : pipeline) {
    auto nodeOp = node[0].asString();
    auto func = OpRegistry::getInstance().get(nodeOp);
    if (!func) {
      CRYSTAL_LOG(ERROR) << nodeOp << " op not registered";
      return false;
    }
  }
  {
    OpContext pipelineCtx;
    pipelineCtx.view = nullptr;
    pipelineCtx.param = pipeline;
    contexts_.emplace("pipeline", pipelineCtx);
  }
  auto task = [&]() {
    auto& pipelineCtx = contexts_["pipeline"];
    for (auto& node : pipelineCtx.param) {
      auto nodeOp = node[0].asString();
      auto param = node[1];
      auto func = OpRegistry::getInstance().get(nodeOp);
      OpContext ctx;
      ctx.view = pipelineCtx.view;
      ctx.param = param;
      QueryOpVarVisitor visitor(&ctx);
      std::visit(visitor, *func);
    }
  };
  taskflow_.emplace(task).name("pipeline");
  return true;
}

void Graph::run(DataView* view) {
  for (auto& kv : contexts_) {
    kv.second.view = view;
  }
  executor_->run(taskflow_).wait();
}

}  // namespace crystal
