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

#include "crystal/operator/generic/Serialize.h"

#include "crystal/operator/generic/ConvertToDynamic.h"

namespace crystal {
namespace op {

#define CRYSTAL_SERIALIZE_TYPE_STR(type) #type

static const char* sSerializeTypeStrings[] = {
  CRYSTAL_SERIALIZE_TYPE_GEN(CRYSTAL_SERIALIZE_TYPE_STR)
};

#undef CRYSTAL_DATA_TYPE_STR

const char* serializeTypeToString(int type) {
  return sSerializeTypeStrings[type];
}

std::string Serialize::compose(DataView& view) const {
  dynamic d = view | toDynamicObject(tableMode_);
  switch (type_) {
    case CSON: return prettify_ ? toPrettyCson(d) : toCson(d);
    case JSON: return prettify_ ? toPrettyJson(d) : toJson(d);
  }
  return "";
}

dynamic Serialize::toDynamic() const {
  return dynamic::object
    ("Serialize", dynamic::object
     ("type", serializeTypeToString(type_))
     ("tableMode", tableMode_)
     ("prettify", prettify_));
}

} // namespace op
} // namespace crystal
