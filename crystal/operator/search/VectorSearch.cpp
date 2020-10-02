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

#include "crystal/operator/search/VectorSearch.h"

#include "crystal/math/Matrix.h"
#include "crystal/storage/index/vector/VectorIndex.h"

namespace crystal {
namespace op {

void searchVectorIndex(DataView& view,
                       const std::string& indexName,
                       const std::string& appendDistanceField,
                       uint16_t segment,
                       int64_t n,
                       Span<float> x,
                       int64_t k) {
  DocumentArray& vec = *view.getBaseTable();
  auto vIndex = reinterpret_cast<VectorIndex*>(
      vec.object()->table()->getIndex(indexName, segment)->index());
  if (!vIndex) {
    CRYSTAL_LOG(ERROR) << "index '" << indexName << "[" << segment << "]' not found";
    return;
  }
  int64_t dim = vIndex->config()->vectorMeta().dimension;
  int64_t querydim = x.size() / n;
  if (querydim != dim) {
    CRYSTAL_LOG(ERROR) << "unmatch dimension: " << querydim << "!=" << dim;
    return;
  }
  CRYSTAL_LOG(DEBUG) << "search vector index with " << n << " vector(s): "
      << Matrix<float>(x, querydim) << ", k: " << k;

  view.appendField(appendDistanceField, true);
  size_t distanceCol = view.getIndexOfField(appendDistanceField);

  float distances[n * k];
  int64_t labels[n * k];
  vIndex->vsearch(n, x.data(), k, distances, labels);
  size_t row = vec.docs().size();
  for (int64_t i = 0; i < n; ++i) {
    uint16_t offset = vec.getTokenCount();
    for (int64_t j = 0; j < k; ++j) {
      int64_t id = labels[i * k + j];
      if (id != -1) {
        auto& doc = vec.docs().emplaceTemp(vec.object(), offset, id);
#if CRYSTAL_CHECK_DELETE
        if (doc.isValid()) {
#endif
          vec.docs().increment();
          view.set(row++, distanceCol, distances[i * k + j]);
#if CRYSTAL_CHECK_DELETE
        }
#endif
      }
    }
    vec.incrementTokenCount();
  }
}

DataView& VectorSearch::compose(DataView& view) const {
  if (!view.getBaseTable()) {
    CRYSTAL_LOG(ERROR) << "no base table";
    return view;
  }
  std::string key = key_;
  DataType type = view.getObject()->getFieldType(key);
  if (type == DataType::UNKNOWN) {
    CRYSTAL_LOG(ERROR) << "table not have field: " << key;
    return view;
  }
  searchVectorIndex(view, key, appendDistanceField_, segment_, n_, x_, k_);
  view.docIndex().resize(view.getBaseTable()->getDocCount());
  CRYSTAL_LOG(DEBUG) << "vsearch '" << key << "' got "
      << view.getRowCount() << " docs";
  return view;
}

dynamic VectorSearch::toDynamic() const {
  return dynamic::object
    ("VectorSearch", dynamic::object
     ("n", n_)
     ("x", x_.toString())
     ("key", key_)
     ("appendDistanceField", appendDistanceField_)
     ("segment", segment_)
     ("k", k_));
}

} // namespace op
} // namespace crystal
