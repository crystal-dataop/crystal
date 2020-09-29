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

#include "crystal/storage/index/Index.h"

#include "crystal/storage/index/IndexType.h"
#include "crystal/storage/index/bitmap/BitmapIndex.h"

namespace crystal {

Index::Index(const IndexConfig& config)
    : config_(&config) {
}

bool Index::init(MemoryManager* memory) {
  IndexType type = stringToIndexType(config_->type().c_str());
  switch (type) {
    case IndexType::kBitmap:
      index_ = std::unique_ptr<IndexBase>(new BitmapIndex(config_));
      break;
    default:
      CRYSTAL_LOG(ERROR) << "unsupport index type: " << config_->type();
      return false;
  }
  if (!index_->init(memory)) {
    CRYSTAL_LOG(ERROR) << "init index failed";
    return false;
  }
  return true;
}

}  // namespace crystal
