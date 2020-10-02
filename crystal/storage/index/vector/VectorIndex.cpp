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

#include "crystal/storage/index/vector/VectorIndex.h"

#include <faiss/Index.h>
#include <faiss/index_factory.h>
#include <faiss/index_io.h>
#include <faiss/MetricType.h>

#include "crystal/foundation/Logging.h"

namespace crystal {

bool VectorIndex::init(MemoryManager* memory) {
  auto& meta = config_->vectorMeta();
  switch (meta.type) {
    case VectorType::Faiss: {
      Memory* mem = memory->getMemory(MemoryType::kMemFaiss, &meta);
      index_ = reinterpret_cast<faiss::Index*>(mem->address(kMemStart));
      break;
    }
  }
  vectors_.dimension = meta.dimension;
  return true;
}

AnyPostingList VectorIndex::getPostingList(uint64_t) {
  return VectorPostingList(this, &vectors_);
}

void VectorIndex::createPostingList(uint64_t) {
}

void VectorIndex::updatePostingList(uint64_t, void*) {
}

void VectorIndex::vadd(bool flush) {
  const float* x = vectors_.data.data();
  int64_t n = vectors_.data.size() / vectors_.dimension;
  if (flush) {
    if (!index_->is_trained) {
      index_->train(n, x);
    }
    index_->add(n, x);
    vectors_.data.clear();
    vectors_.firstId = index_->ntotal;
  } else {
    if (!index_->is_trained) {
      if (n >= config_->vectorMeta().trainSize) {
        index_->train(n, x);
      }
    }
    if (index_->is_trained && n >= config_->vectorMeta().batchSize) {
      index_->add(n, x);
      vectors_.data.clear();
      vectors_.firstId = index_->ntotal;
    }
  }
}

void VectorIndex::vsearch(
    int64_t n, const float* x, int64_t k,
    float* distances, int64_t* labels) const {
  /** query n vectors of dimension d to the index.
   *
   * return at most k vectors. If there are not enough results for a
   * query, the result array is padded with -1s.
   *
   * @param x           input vectors to search, size n * d
   * @param labels      output labels of the NNs, size n*k
   * @param distances   output pairwise distances, size n*k
   */
  index_->search(n, x, k, distances, labels);
}

}  // namespace crystal
