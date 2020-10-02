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
#include "crystal/operator/search/Search.h"
#include "crystal/operator/search/VectorSearch.h"

namespace crystal {

static OpRegistryReceiver<QueryOp> searchQueryOp(
    "Search",
    [](OpContext& ctx) {
      auto tokens = ctx.param["tokens"].asString();
      auto key = ctx.param.getDefault("key", "").asString();
      auto limit = ctx.param.getDefault("payloadLimit", -1).asInt();
      auto mergeType = op::stringToMergeType(
          ctx.param.getDefault("mergeType", "Append").asString().c_str());
      *ctx.view | search(tokens, key, limit, mergeType);
    });

static OpRegistryReceiver<QueryOp> vSearchQueryOp(
    "VectorSearch",
    [](OpContext& ctx) {
      auto n = ctx.param["n"].asInt();
      auto x = ctx.param["x"].asString();
      auto key = ctx.param["key"].asString();
      auto appendDistanceField = ctx.param["appendDistanceField"].asString();
      auto segment = ctx.param["segment"].asInt();
      auto k = ctx.param["k"].asInt();
      auto xVector = toVector<float>(x);
      Span<float> xSpan(xVector.data(), xVector.size());
      *ctx.view | op::vSearch(n, xSpan, key, appendDistanceField, segment, k);
    });

}  // namespace crystal
