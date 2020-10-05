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

#include "crystal/operator/generic/ConvertToRecordArray.h"

#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {
namespace op {

struct ItemMeta {
  std::string name;
  size_t jIndex;
  ItemType type;
};

void serializeItemToRecord(
    const DataView& view, size_t i, const ItemMeta& meta,
    int tag, Record& record) {
  switch (meta.type.type) {
#define ENCODE(_type, enum_type)                              \
    case DataType::enum_type: {                               \
      if (meta.type.count != 1) {                             \
        auto value = view.get<Array<_type>>(i, meta.jIndex);  \
        record.set<Array<_type>>(tag, *value);                \
      } else {                                                \
        auto value = view.get<_type>(i, meta.jIndex);         \
        record.set<_type>(tag, *value);                       \
      }                                                       \
    }

    ENCODE(bool, BOOL)
    ENCODE(int8_t, INT8)
    ENCODE(int16_t, INT16)
    ENCODE(int32_t, INT32)
    ENCODE(int64_t, INT64)
    ENCODE(uint8_t, UINT8)
    ENCODE(uint16_t, UINT16)
    ENCODE(uint32_t, UINT32)
    ENCODE(uint64_t, UINT64)
    ENCODE(float, FLOAT)
    ENCODE(double, DOUBLE)
    ENCODE(std::string_view, STRING)

#undef ENCODE

    case DataType::UNKNOWN:
      CRYSTAL_LOG(ERROR) << "unsupport data type: "
          << dataTypeToString(meta.type.type);
      break;
  }
}

RecordArray ConvertToRecordArray::compose(DataView& view) const {
  std::vector<ItemMeta> metas;
  for (auto p : view.fieldIndex().index) {
    ItemMeta meta = {
      p.first,
      p.second,
      view.getColType(p.second),
    };
    metas.push_back(meta);
  }
  RecordMeta recordMeta;
  int tag = 1;
  for (auto& meta : metas) {
    recordMeta.addMeta(FieldMeta(meta.name,
                                 tag++,
                                 meta.type.type,
                                 sizeOf(meta.type.type) * 8,
                                 meta.type.count));
  }
  RecordArray out(recordMeta);
  for (auto i : view.docIndex()) {
    Record record = out.createRecord();
    tag = 1;
    for (auto& meta : metas) {
      serializeItemToRecord(view, i, meta, tag++, record);
    }
  }
  return out;
}

dynamic ConvertToRecordArray::toDynamic() const {
  return dynamic::object
    ("ConvertToRecordArray", dynamic::object);
}

} // namespace op
} // namespace crystal
