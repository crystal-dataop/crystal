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

#include <vector>

#include "crystal/memory/ChunkedMemory.h"
#include "crystal/memory/SimpleAllocator.h"
#include "crystal/serializer/record/Record.h"

namespace crystal {

template <class Alloc>
class RecordArrayBase {
 public:
  RecordArrayBase(const RecordMeta& meta)
      : recordMeta_(meta),
        accessor_(recordMeta_) {}

  virtual ~RecordArrayBase() {
    release();
  }

  bool init(Memory* memory) {
    return alloc_.init(memory);
  }

  const RecordMeta& recordMeta() const {
    return recordMeta_;
  }

  Record createRecord() {
    Record record;
    int64_t offset = alloc_.allocate(accessor_.bufferSize());
    if (offset == 0) {
      return record;
    }
    offsets_.push_back(offset);
    void* buf = alloc_.address(offset);
    memset(buf, 0, accessor_.bufferSize());
    record.init(&recordMeta_, &accessor_, &alloc_, buf);
    record.reset();
    return record;
  }

  void release() {
    Record record(&recordMeta_, &accessor_, &alloc_);
    for (int64_t offset : offsets_) {
      record.setBuffer(alloc_.address(offset));
      record.reset();
      alloc_.deallocate(offset);
    }
    offsets_.clear();
  }

 private:
  RecordMeta recordMeta_;
  Accessor accessor_;
  Alloc alloc_;
  std::vector<int64_t> offsets_;
};

class RecordArray : public RecordArrayBase<SimpleAllocator> {
 public:
  RecordArray(const RecordMeta& meta)
      : RecordArrayBase<SimpleAllocator>(meta) {
    init(&memory_);
  }

  std::string toBuffer() const {
    return memory_.toBuffer();
  }

 private:
  ChunkedMemory memory_;
};

} // namespace crystal
