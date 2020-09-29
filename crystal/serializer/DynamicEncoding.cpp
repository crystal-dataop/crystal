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

#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {

namespace detail {

template <class T>
void parseField(const dynamic& j, Record& value, const FieldMeta& meta) {
  if (meta.isArray()) {
    if (!j.isArray()) {
      CRYSTAL_THROW(RuntimeError, "not array value");
    }
    size_t n = j.size();
    if (meta.isVarArray()) {
      value.buildVarArray(meta, n);
    } else if (meta.count() != n) {
      CRYSTAL_THROW(LengthError, "unequal array size: ", meta.count(), "!=", n);
    }
    if (n > 0) {
      Array<T> a = value.get<Array<T>>(meta);
      decode(j, a);
    }
  } else {
    T v;
    decode(j, v);
    value.set<T>(meta, v);
  }
}

} // namespace detail

void decode(const dynamic& j, Record& value) {
  if (!j.isObject()) {
    return;
  }
  for (auto& kv : j.items()) {
    auto meta = value.recordMeta()->getMeta(kv.first.asString());
    if (meta) {
      switch (meta->type()) {
#define DECODE(type, enum_type)                         \
  case DataType::enum_type:                             \
    detail::parseField<type>(kv.second, value, *meta);  \
    break;

        DECODE(bool, BOOL)
        DECODE(int8_t, INT8)
        DECODE(int16_t, INT16)
        DECODE(int32_t, INT32)
        DECODE(int64_t, INT64)
        DECODE(uint8_t, UINT8)
        DECODE(uint16_t, UINT16)
        DECODE(uint32_t, UINT32)
        DECODE(uint64_t, UINT64)
        DECODE(float, FLOAT)
        DECODE(double, DOUBLE)
        DECODE(std::string_view, STRING)

#undef DECODE

        case DataType::UNKNOWN:
          CRYSTAL_LOG(ERROR) << "unsupport data type: "
              << dataTypeToString(meta->type());
          break;
      }
    }
  }
}

} // namespace crystal
