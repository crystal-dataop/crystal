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

#include "crystal/record/DynamicEncoding.h"

namespace crystal {

dynamic encode(const untyped_tuple& value) {
  dynamic j = dynamic::array;
  for (size_t i = 0; i < value.size(); ++i) {
    auto& em = value.meta_[i];
    switch (em.type) {
#define CASE(dt, t)                                   \
      case DataType::dt: {                            \
        if (em.count == 1) {                          \
          j.push_back(encode(value.get<t>(i)));       \
        } else {                                      \
          j.push_back(encode(value.getRange<t>(i)));  \
        }                                             \
        break;                                        \
      }

      CASE(BOOL, bool)
      CASE(INT8, int8_t)
      CASE(INT16, int16_t)
      CASE(INT32, int32_t)
      CASE(INT64, int64_t)
      CASE(UINT8, uint8_t)
      CASE(UINT16, uint16_t)
      CASE(UINT32, uint32_t)
      CASE(UINT64, uint64_t)
      CASE(FLOAT, float)
      CASE(DOUBLE, double)
      CASE(STRING, string)
      CASE(TUPLE, untyped_tuple)
      default: break;

#undef CASE
    }
  }
  return j;
}

dynamic encode(const untyped_tuple::meta& value) {
  dynamic j = dynamic::array;
  for (auto& em : value) {
    dynamic type = em.type == DataType::TUPLE
      ? encode(untyped_tuple::meta{em.submeta})
      : dataTypeToString(em.type);
    j.push_back(dynamic::object("type", type)("count", em.count));
  }
  return j;
}

dynamic mergeTupleValueAndMeta(const dynamic& value, const dynamic& meta) {
  dynamic j = dynamic::array;
  if (value.size() != meta.size()) {
    return nullptr;
  }
  for (size_t i = 0; i < meta.size(); ++i) {
    dynamic o = meta[i];
    if (o["type"].isArray()) {
      if (o["count"] == 1) {
        o["value"] = mergeTupleValueAndMeta(value[i], o["type"]);
      } else {
        dynamic a = dynamic::array;
        for (size_t k = 0; k < value[i].size(); ++k) {
          a.push_back(mergeTupleValueAndMeta(value[i][k], o["type"]));
        }
        o["value"] = a;
      }
      j.push_back(o);
    } else {
      o["value"] = value[i];
      j.push_back(o);
    }
  }
  return j;
}

void decode(const dynamic& j, untyped_tuple::meta& value) {
  value.reserve(value.size());
  for (auto& o : j) {
    if (o["type"].isArray()) {
      untyped_tuple::meta submeta;
      decode(o["type"], submeta);
      value.add_type<untyped_tuple>(submeta, o["count"].getInt());
      continue;
    }
    switch (stringToDataType(o["type"].c_str())) {
#define CASE(dt, t)                                                     \
      case DataType::dt:                                                \
        value.add_type<t>(untyped_tuple::meta{}, o["count"].getInt());  \
        break;

      CASE(BOOL, bool)
      CASE(INT8, int8_t)
      CASE(INT16, int16_t)
      CASE(INT32, int32_t)
      CASE(INT64, int64_t)
      CASE(UINT8, uint8_t)
      CASE(UINT16, uint16_t)
      CASE(UINT32, uint32_t)
      CASE(UINT64, uint64_t)
      CASE(FLOAT, float)
      CASE(DOUBLE, double)
      CASE(STRING, string)

      case DataType::TUPLE:
      default: break;

#undef CASE
    }
  }
}

void decode(const dynamic& j, untyped_tuple& value) {
  decode(j, value.meta_);
  for (uint32_t i = 0; i < value.size(); ++i) {
    auto& em = value.meta_[i];
    switch (em.type) {
#define CASE(dt, t)                               \
      case DataType::dt: {                        \
        if (em.count == 0) {                      \
          decode(j[i], value.get<vector<t>>(i));  \
        } else {                                  \
          auto& a = j[i];                         \
          auto r = value.getRange<t>(i);          \
          for (size_t k = 0; k < r.size(); ++k) { \
            decode(a[k], r[k]);                   \
          }                                       \
        }                                         \
        break;                                    \
      }

      CASE(BOOL, bool)
      CASE(INT8, int8_t)
      CASE(INT16, int16_t)
      CASE(INT32, int32_t)
      CASE(INT64, int64_t)
      CASE(UINT8, uint8_t)
      CASE(UINT16, uint16_t)
      CASE(UINT32, uint32_t)
      CASE(UINT64, uint64_t)
      CASE(FLOAT, float)
      CASE(DOUBLE, double)
      CASE(STRING, string)
      CASE(TUPLE, untyped_tuple)
      default: break;

#undef CASE
    }
  }
}

} // namespace crystal
