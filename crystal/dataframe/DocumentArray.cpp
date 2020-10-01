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

#include "crystal/dataframe/DocumentArray.h"

namespace crystal {

void DocumentArray::trim(const U32IndexArray& docIndex) {
  DocStorageArray docs;
  for (auto i : docIndex) {
    docs.emplace(std::move(docs_[i]));
  }
  docs_.swap(docs);
}

bool DocumentArray::merge(DocumentArray& other) {
  if (object_ != other.object_ ||
      object_->name() != other.object_->name()) {
    return false;
  }
  size_t n = other.docs_.size();
  for (size_t i = 0; i < n; ++i) {
    other.docs_[i].setTokenOffset(tokenCount_ + other.docs_[i].tokenOffset());
  }
  tokenCount_ += other.tokenCount_;
  docs_.merge(other.docs_);
  return true;
}

void DocumentArray::copyTo(DynamicTable& table,
                           const U32IndexArray& docIndex,
                           const std::vector<size_t>& fieldIndex) {
  for (auto i : docIndex) {
    for (auto j : fieldIndex) {
      ItemType itemType = getColType(j);
      switch (itemType.type) {
#define COPY(type, enum_type)                               \
        case DataType::enum_type: {                         \
          if (itemType.count != 1) {                        \
            table.set(i, j, *getUnsafe<Array<type>>(i, j)); \
          } else {                                          \
            table.set(i, j, *getUnsafe<type>(i, j));        \
          }                                                 \
          break;                                            \
        }

        COPY(bool, BOOL)
        COPY(int8_t, INT8)
        COPY(int16_t, INT16)
        COPY(int32_t, INT32)
        COPY(int64_t, INT64)
        COPY(uint8_t, UINT8)
        COPY(uint16_t, UINT16)
        COPY(uint32_t, UINT32)
        COPY(uint64_t, UINT64)
        COPY(float, FLOAT)
        COPY(double, DOUBLE)
        COPY(std::string_view, STRING)

#undef COPY

        default:
          CRYSTAL_LOG(ERROR) << "unsupport data type: "
              << dataTypeToString(itemType.type);
          break;
      }
    }
  }
}

}  // namespace crystal
