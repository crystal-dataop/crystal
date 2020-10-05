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

#include "crystal/operator/generic/ConvertToDynamic.h"

#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {
namespace op {

struct ItemMeta {
  std::string name;
  size_t jIndex;
  ItemType type;
};

dynamic serializeItemToDynamic(
    const DataView& view, size_t i, const ItemMeta& meta) {
  switch (meta.type.type) {
#define ENCODE(_type, enum_type)                              \
    case DataType::enum_type: {                               \
      if (meta.type.count != 1) {                             \
        auto value = view.get<Array<_type>>(i, meta.jIndex);  \
        return value ? encode(*value) : nullptr;              \
      } else {                                                \
        auto value = view.get<_type>(i, meta.jIndex);         \
        return value ? encode(*value) : nullptr;              \
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
  return nullptr;
}

dynamic ConvertToDynamic::compose(DataView& view) const {
  std::vector<ItemMeta> metas;
  for (auto p : view.fieldIndex().index) {
    ItemMeta meta = {
      p.first,
      p.second,
      view.getColType(p.second),
    };
    metas.push_back(meta);
  }
  dynamic out;
  if (tableMode_) {
    out = dynamic::object
      ("name", dynamic::array)
      ("type", dynamic::array)
      ("data", dynamic::array);
    for (auto& meta : metas) {
      out["name"].push_back(meta.name);
      out["type"].push_back(dataTypeToString(meta.type.type));
    }
    for (auto i : view.docIndex()) {
      dynamic j = dynamic::array;
      for (auto& meta : metas) {
        j.push_back(serializeItemToDynamic(view, i, meta));
      }
      out["data"].push_back(j);
    }
  } else {
    out = dynamic::array;
    for (auto i : view.docIndex()) {
      dynamic j = dynamic::object;
      for (auto& meta : metas) {
        j.insert(meta.name, serializeItemToDynamic(view, i, meta));
      }
      out.push_back(j);
    }
  }
  return out;
}

dynamic ConvertToDynamic::toDynamic() const {
  return dynamic::object
    ("ConvertToDynamic", dynamic::object
     ("tableMode", tableMode_));
}

} // namespace op
} // namespace crystal
