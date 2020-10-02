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

#include "crystal/operator/Operator.h"
#include "crystal/operator/search/detail/Search.h"
#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {
namespace op {

template <class Container>
class Search : public Operator<Search<Container>> {
  Container tokens_;
  std::string key_;
  size_t payloadLimit_;
  MergeType mergeType_;

 public:
  Search(const Container& tokens,
         const std::string& key,
         size_t payloadLimit,
         MergeType mergeType)
      : tokens_(tokens),
        key_(key),
        payloadLimit_(payloadLimit),
        mergeType_(mergeType) {}

  DataView& compose(DataView& view) const;

  dynamic toDynamic() const;
};

template <class Container>
inline Search<Container> search(
    const Container& tokens,
    const std::string& key = "",
    size_t payloadLimit = -1,
    MergeType mergeType = MergeType::kAppend) {
  return Search<Container>(tokens, key, payloadLimit, mergeType);
}

//////////////////////////////////////////////////////////////////////

template <class Container>
DataView& Search<Container>::compose(DataView& view) const {
  if (!view.getBaseTable()) {
    CRYSTAL_LOG(ERROR) << "no base table";
    return view;
  }
  std::string key = key_;
  if (key == "") {
    key = view.getObject()->key();
  }
  DataType type = view.getObject()->getFieldType(key);
  if (type == DataType::UNKNOWN) {
    CRYSTAL_LOG(ERROR) << "table not have field: " << key;
    return view;
  }
  size_t indexNo = view.getObject()->table()->getNoOfIndex(key);
  if (indexNo != size_t(-1)) {
    DocumentArray* index = view.getBaseTable();
    switch (type) {
#define SEARCH(type, enum_type)                                       \
      case DataType::enum_type:                                       \
        msearchIndex<type>(                                           \
            *index, key, indexNo, tokens_, payloadLimit_, mergeType_);\
        break;

      SEARCH(int8_t, INT8)
      SEARCH(int16_t, INT16)
      SEARCH(int32_t, INT32)
      SEARCH(int64_t, INT64)
      SEARCH(uint8_t, UINT8)
      SEARCH(uint16_t, UINT16)
      SEARCH(uint32_t, UINT32)
      SEARCH(uint64_t, UINT64)
      SEARCH(std::string_view, STRING)

#undef SEARCH

      default:
        CRYSTAL_LOG(ERROR) << "unsupport key type: " << dataTypeToString(type);
        break;
    }
    view.docIndex().resize(index->getDocCount());
  }
  else if (view.getObject()->hasKV()) {
    if (mergeType_ != MergeType::kAppend) {
      CRYSTAL_LOG(ERROR) << "search kv only support append mode";
      return view;
    }
    DocumentArray* kv = view.getBaseTable();
    switch (type) {
#define SEARCH(type, enum_type)         \
      case DataType::enum_type:         \
        msearchKV<type>(*kv, tokens_);  \
        break;

      SEARCH(int8_t, INT8)
      SEARCH(int16_t, INT16)
      SEARCH(int32_t, INT32)
      SEARCH(int64_t, INT64)
      SEARCH(uint8_t, UINT8)
      SEARCH(uint16_t, UINT16)
      SEARCH(uint32_t, UINT32)
      SEARCH(uint64_t, UINT64)
      SEARCH(std::string_view, STRING)

#undef SEARCH

      default:
        CRYSTAL_LOG(ERROR) << "unsupport key type: " << dataTypeToString(type);
        break;
    }
    view.docIndex().resize(kv->getDocCount());
  }
  else {
    CRYSTAL_LOG(ERROR) << "field '" << key << "' is not index name and no kv";
    return view;
  }
  CRYSTAL_LOG(DEBUG) << "search '" << key << "' got "
      << view.getRowCount() << " docs";
  return view;
}

template <class Container>
dynamic Search<Container>::toDynamic() const {
  return dynamic::object
    ("Search", dynamic::object
     ("tokens", encode(tokens_))
     ("key", key_)
     ("payloadLimit", payloadLimit_)
     ("mergeType", mergeTypeToString(mergeType_)));
}

} // namespace op
} // namespace crystal
