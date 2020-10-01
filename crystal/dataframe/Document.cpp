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

#include "crystal/dataframe/Document.h"

namespace crystal {

Document::Document(
    const ExtendedTable* table,
    uint16_t tokenOffset,
    uint64_t id)
    : table_(table),
      id_(id),
      kv_(nullptr),
      value_(0),
      tokenOffset_(tokenOffset) {
  kv_ = table_->table()->getKVById(id);
  validator_.checkValid(id_, kv_);
  if (validator_.isValid) {
    value_ = uintptr_t(kv_->getRecordPtr(id_));
  }
}

Document::Document(
    const ExtendedTable* table,
    uint16_t tokenOffset,
    const char* payload,
    IndexBase* index,
    uint16_t indexNo)
    : table_(table),
      id_(0),
      kv_(nullptr),
      value_(0),
      tokenOffset_(tokenOffset) {
  const uint64_t* p = reinterpret_cast<const uint64_t*>(payload);
  id_ = *p;
  kv_ = table_->table()->getKVById(id_);
  validator_.checkValid(id_, kv_);
  if (validator_.isValid) {
    value_ = uintptr_t(kv_->getRecordPtr(id_));
    index_.index = index;
    index_.indexNo = indexNo;
    index_.payload = uintptr_t(p + 1);
  }
}

void Document::Validator::checkValid(uint64_t id, KV* kv, int i) {
  if (isValid) {
    if (i >= 0) {
      if (checkMask & (1 << i)) {
        return;
      }
      checkMask |= (1 << i);
    }
    if (id == uint64_t(-1)) {
      CRYSTAL_LOG(DEBUG) << "id=-1" << (kv ? "@" + kv->config().name() : "")
          << " is null";
      isValid = false;
      return;
    }
#if CRYSTAL_CHECK_DELETE
    if (kv && !kv->exist(id)) {
      CRYSTAL_LOG(DEBUG) << "id=" << id << "@" << kv->config().name()
          << " is deleted";
      isValid = false;
      return;
    }
#endif
  }
}

}  // namespace crystal
