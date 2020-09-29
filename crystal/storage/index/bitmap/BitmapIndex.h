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

#include "crystal/storage/index/IndexBase.h"
#include "crystal/storage/index/bitmap/BitmapPostingList.h"
#include "crystal/storage/kv/HashMap.h"

namespace crystal {

class BitmapIndex : public IndexBase {
 public:
  explicit BitmapIndex(const IndexConfig* config)
      : IndexBase(config),
        hashMap_(config->bucket()) {}

  virtual ~BitmapIndex() {}

  bool init(MemoryManager* memory) override;

  AnyPostingList getPostingList(uint64_t key) override;
  void createPostingList(uint64_t key) override;
  void updatePostingList(uint64_t key, void* meta) override;

 private:
  HashMap<uint64_t, BitmapPostingList::Meta> hashMap_;
};

}  // namespace crystal
