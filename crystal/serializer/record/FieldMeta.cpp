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

#include "crystal/serializer/record/FieldMeta.h"

namespace crystal {

FieldMeta::FieldMeta()
    : name_("invalid"),
      tag_(0),
      type_(DataType::UNKNOWN),
      bits_(0),
      count_(1),
      compact_(false) {}

FieldMeta::FieldMeta(const std::string& name,
                     int tag,
                     DataType type,
                     size_t bits,
                     size_t count,
                     const dynamic& dflt)
    : name_(name),
      tag_(tag),
      type_(type),
      bits_(bits),
      count_(count) {
  switch (type_) {
#define DEFAULT(type, enum_type)                                      \
    case DataType::enum_type:                                         \
      default_ = dflt.isNull() ? type(0) : to<type>(dflt.asString()); \
      break;

    DEFAULT(bool, BOOL)
    DEFAULT(int8_t, INT8)
    DEFAULT(int16_t, INT16)
    DEFAULT(int32_t, INT32)
    DEFAULT(int64_t, INT64)
    DEFAULT(uint8_t, UINT8)
    DEFAULT(uint16_t, UINT16)
    DEFAULT(uint32_t, UINT32)
    DEFAULT(uint64_t, UINT64)
    DEFAULT(float, FLOAT)
    DEFAULT(double, DOUBLE)

    case DataType::STRING:
      default_ = dflt.isNull() ? "" : dflt.asString();
      break;
    default: break;
  }
  compact_ = isBool(type_) || (isIntegral(type_)
                               && bits_ > 0
                               && bits_ < sizeOf(type_) * 8);
}

dynamic FieldMeta::toDynamic() const {
  return dynamic::object
    ("name", name_)
    ("tag", tag_)
    ("type", dataTypeToString(type_))
    ("bits", bits_)
    ("count", count_)
    ("compact", compact_)
    ("default", default_);
}

std::string FieldMeta::toString() const {
  return toCson(toDynamic());
}

}  // namespace crystal
