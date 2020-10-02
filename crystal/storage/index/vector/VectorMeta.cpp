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

#include "crystal/storage/index/vector/VectorMeta.h"

#include <map>
#include <strings.h>

#include <faiss/MetricType.h>

#include "crystal/foundation/UnionBasedStatic.h"

namespace crystal {

#define CRYSTAL_VECTOR_TYPE_STR(type) #type

namespace {

static const char* sVectorTypeStrings[] = {
  CRYSTAL_VECTOR_TYPE_GEN(CRYSTAL_VECTOR_TYPE_STR)
};

}

const char* vectorTypeToString(VectorType type) {
  return sVectorTypeStrings[static_cast<unsigned>(type)];
}

namespace {

#define CRYSTAL_FAISS_METRIC_PAIR(m) { #m, faiss::MetricType::METRIC_##m }

typedef std::map<const char*, int> MetricMap;
DEFINE_UNION_STATIC_CONST_NO_INIT(MetricMap, Map, s_metricMap);

__attribute__((__constructor__))
void initMetricMap() {
  new (const_cast<MetricMap*>(&s_metricMap.data)) MetricMap {
    CRYSTAL_FAISS_METRIC_PAIR(INNER_PRODUCT),
    CRYSTAL_FAISS_METRIC_PAIR(L2),
    CRYSTAL_FAISS_METRIC_PAIR(L1),
    CRYSTAL_FAISS_METRIC_PAIR(Linf),
    CRYSTAL_FAISS_METRIC_PAIR(Lp),
    CRYSTAL_FAISS_METRIC_PAIR(Canberra),
    CRYSTAL_FAISS_METRIC_PAIR(BrayCurtis),
    CRYSTAL_FAISS_METRIC_PAIR(JensenShannon)
  };
}

#undef CRYSTAL_FAISS_METRIC_PAIR

}

int stringToFaissMetric(const char* str) {
  for (auto& p : s_metricMap.data) {
    if (strcasecmp(str, p.first) == 0) {
      return p.second;
    }
  }
  return -1;
}

}  // namespace crystal
