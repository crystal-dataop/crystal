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

#include "crystal/memory/SysAllocator.h"
#include "crystal/serializer/DynamicEncoding.h"
#include "crystal/serializer/record/RecordArray.h"
#include "crystal/serializer/record/RecordConfig.h"

namespace crystal {

template <class Alloc = SysAllocator>
class RecordBuilder {
 public:
  RecordBuilder(const RecordMeta& meta);
  RecordBuilder(const RecordConfig& config, bool addId = false);
  RecordBuilder(const dynamic& j, bool addId = false);

  virtual ~RecordBuilder();

  bool init(Memory* memory);

  Record build();

  Record build(const dynamic& j);

  void release();

 private:
  RecordArrayBase<Alloc> recordArray_;
};

//////////////////////////////////////////////////////////////////////

template <class Alloc>
RecordBuilder<Alloc>::RecordBuilder(const RecordMeta& meta)
    : recordArray_(meta) {}

template <class Alloc>
RecordBuilder<Alloc>::RecordBuilder(const RecordConfig& config, bool addId)
    : recordArray_(buildRecordMeta(config, addId)) {}

template <class Alloc>
RecordBuilder<Alloc>::RecordBuilder(const dynamic& j, bool addId)
    : recordArray_(buildRecordMeta(parseRecordConfig(j), addId)) {}

template <class Alloc>
RecordBuilder<Alloc>::~RecordBuilder() {
  release();
}

template <class Alloc>
bool RecordBuilder<Alloc>::init(Memory* memory) {
  return recordArray_.init(memory);
}

template <class Alloc>
Record RecordBuilder<Alloc>::build() {
  return recordArray_.createRecord();
}

template <class Alloc>
Record RecordBuilder<Alloc>::build(const dynamic& j) {
  Record record = build();
  decode(j, record);
  return record;
}

template <class Alloc>
void RecordBuilder<Alloc>::release() {
  recordArray_.release();
}

} // namespace crystal
