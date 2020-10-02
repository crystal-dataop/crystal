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

#include <optional>

#include "crystal/storage/table/ExtendedTable.h"

#ifndef CRYSTAL_CHECK_DELETE
#define CRYSTAL_CHECK_DELETE 1
#endif

namespace crystal {

class Document {
 public:
  Document(const ExtendedTable* table,
           uint16_t tokenOffset,
           uint64_t id);

  Document(const ExtendedTable* table,
           uint16_t tokenOffset,
           const char* payload,
           IndexBase* index,
           uint16_t indexNo);

  virtual ~Document() {}

  Document(const Document&) = delete;
  Document& operator=(const Document&) = delete;

  Document(Document&&) = default;
  Document& operator=(Document&&) = default;

  uint16_t tokenOffset() const;
  void setTokenOffset(uint16_t tokenOffset);

  uint64_t id() const;

  bool isNull() const;
  bool isValid() const;

  explicit operator bool() const;

  template <class T>
  std::optional<T> get(size_t i) const;

  struct IndexAddr {
    IndexBase* index;
    uint16_t indexNo;
    uintptr_t payload : 48;
  };

 private:
  struct Validator {
    bool isValid : 1;
    uint64_t checkMask : 63;

    Validator() : isValid(true), checkMask(0) {}

    void checkValid(uint64_t id, KV* kv, int i = -1);
  };

  mutable Validator validator_;
  const ExtendedTable* table_;
  uint64_t id_;
  KV* kv_;
  uintptr_t value_ : 48;
  uint16_t tokenOffset_;
  IndexAddr index_;
};

//////////////////////////////////////////////////////////////////////

inline uint16_t Document::tokenOffset() const {
  return tokenOffset_;
}

inline void Document::setTokenOffset(uint16_t tokenOffset) {
  tokenOffset_ = tokenOffset;
}

inline uint64_t Document::id() const {
  return id_;
}

inline bool Document::isNull() const {
  return id_ == uint64_t(-1);
}

inline bool Document::isValid() const {
  return validator_.isValid;
}

inline Document::operator bool() const {
  return isValid();
}

template <class T>
inline std::optional<T> Document::get(size_t i) const {
  if (!validator_.isValid) {
    CRYSTAL_LOG(WARN) << "invalid doc: "
        << "id=" << id_ << "@" << table_->name() << " get failed";
    return std::nullopt;
  }
  auto& fi = table_->fieldInfos()[i];
  switch (fi.type) {
    case FieldInfo::kPayload: {
      Record record = index_.index->createRecord(
          reinterpret_cast<void*>(index_.payload));
      return record.get<T>(fi.meta);
    }
    case FieldInfo::kPayloadAndValue: {
      if (fi.indexNo.test(index_.indexNo)) {
        Record record = index_.index->createRecord(
            reinterpret_cast<void*>(index_.payload));
        return record.get<T>(fi.meta);
      }
    }
    case FieldInfo::kValue: {
      Record record = kv_->createRecord(reinterpret_cast<void*>(value_));
      return record.get<T>(fi.meta);
    }
    case FieldInfo::kRelated: {
      uint64_t rid = *get<uint64_t>(fi.related.ref);
      Table* rtable = fi.related.table;
      validator_.checkValid(rid, rtable->getKVById(rid), fi.related.no);
      if (!validator_.isValid) {
        CRYSTAL_LOG(WARN) << "invalid doc: "
            << "rid=" << rid << "@" << rtable->config().name() << " get failed";
        break;
      }
      Record record = rtable->getKVById(rid)->createRecord();
      rtable->getKVById(rid)->get(rid, record);
      return record.get<T>(fi.meta);
    }
    case FieldInfo::kUnUsed:
      CRYSTAL_LOG(ERROR) << "get unused field: " << fi.meta.name();
      break;
  }
  return std::nullopt;
}

}  // namespace crystal
