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

#include "crystal/memory/MemoryManager.h"
#include "crystal/memory/RecycledAllocator.h"
#include "crystal/serializer/record/Record.h"
#include "crystal/storage/index/AnyPostingList.h"
#include "crystal/storage/index/IndexConfig.h"
#include "crystal/storage/index/PostingAllocator.h"

namespace crystal {

class IndexBase {
 public:
  explicit IndexBase(const IndexConfig* config);
  virtual ~IndexBase() {}

  virtual bool init(MemoryManager* memory) = 0;

  virtual AnyPostingList getPostingList(uint64_t key) = 0;
  virtual void createPostingList(uint64_t key) = 0;
  virtual void updatePostingList(uint64_t key, void* meta) = 0;

  virtual bool add(uint64_t key, const Record& record);
  virtual bool update(uint64_t key, const Record& record);
  virtual bool remove(uint64_t key, const Record& record);
  virtual bool bulkLoad(uint64_t key, const std::vector<Record>& records);

  const IndexConfig* config() const;
  const RecordMeta& recordMeta() const;
  const FieldMeta& keyMeta() const;
  const Accessor& accessor() const;

  PostingAllocator& allocator();
  RecycledAllocator& recycledAllocator();

  Record createRecord(void* buf = nullptr) const;

 protected:
  const IndexConfig* config_{nullptr};
  RecordMeta recordMeta_;
  FieldMeta keyMeta_;
  Accessor accessor_;
  PostingAllocator alloc_;
  mutable RecycledAllocator recycledAlloc_;
};

//////////////////////////////////////////////////////////////////////

inline const IndexConfig* IndexBase::config() const {
  return config_;
}

inline const RecordMeta& IndexBase::recordMeta() const {
  return recordMeta_;
}

inline const FieldMeta& IndexBase::keyMeta() const {
  return keyMeta_;
}

inline const Accessor& IndexBase::accessor() const {
  return accessor_;
}

inline PostingAllocator& IndexBase::allocator() {
  return alloc_;
}

inline RecycledAllocator& IndexBase::recycledAllocator() {
  return recycledAlloc_;
}

inline Record IndexBase::createRecord(void* buf) const {
  return Record(&recordMeta_, &accessor_, &recycledAlloc_, buf);
}

}  // namespace crystal
