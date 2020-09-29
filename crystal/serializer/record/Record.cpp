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

#include "crystal/serializer/record/Record.h"

namespace crystal {

dynamic Record::toDynamic() const {
  if (!isValid()) {
    return nullptr;
  }
  if (!hasData()) {
    return dynamic::object;
  }

  dynamic j = dynamic::object;
  for (auto& meta : *recordMeta_) {
    if (!accessor_->hasField(buf_, meta)) {
      j.insert(meta.name(), nullptr);
      continue;
    }
    switch (meta.type()) {
#define PRINT(type, enum_type)                    \
  case DataType::enum_type:                       \
    j.insert(meta.name(), toDynamic<type>(meta)); \
    break;

      PRINT(bool, BOOL)
      PRINT(int8_t, INT8)
      PRINT(int16_t, INT16)
      PRINT(int32_t, INT32)
      PRINT(int64_t, INT64)
      PRINT(uint8_t, UINT8)
      PRINT(uint16_t, UINT16)
      PRINT(uint32_t, UINT32)
      PRINT(uint64_t, UINT64)
      PRINT(float, FLOAT)
      PRINT(double, DOUBLE)
      PRINT(std::string_view, STRING)

#undef PRINT

      case DataType::UNKNOWN:
        CRYSTAL_THROW(RuntimeError,
                      "unsupport data type: ", dataTypeToString(meta.type()));
        break;
    }
  }
  return j;
}

std::string Record::toString(const json::serialization_opts& opts) const {
  return json::serialize(toDynamic(), opts);
}

}  // namespace crystal
