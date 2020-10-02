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

namespace crystal {
namespace op {

#define CRYSTAL_SERIALIZE_TYPE_GEN(x) \
  x(CSON),                            \
  x(JSON)

class Serialize : public Operator<Serialize> {
  int type_;
  bool tableMode_;
  bool prettify_;

 public:

#define CRYSTAL_SERIALIZE_TYPE_ENUM(type) type

  enum {
    CRYSTAL_SERIALIZE_TYPE_GEN(CRYSTAL_SERIALIZE_TYPE_ENUM)
  };

#undef CRYSTAL_DATA_TYPE_ENUM

  Serialize(int type, bool tableMode, bool prettify)
      : type_(type), tableMode_(tableMode), prettify_(prettify) {}

  std::string compose(DataView& view) const;

  dynamic toDynamic() const;
};

inline Serialize toCson(bool tableMode = false, bool prettify = false) {
  return Serialize(Serialize::CSON, tableMode, prettify);
}

inline Serialize toJson(bool tableMode = false, bool prettify = false) {
  return Serialize(Serialize::JSON, tableMode, prettify);
}

} // namespace op
} // namespace crystal
