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

#include "crystal/type/DataType.h"

namespace crystal {

#define CRYSTAL_DATA_TYPE_STR(type) #type

static const char* sDataTypeStrings[] = {
  CRYSTAL_DATA_TYPE_GEN(CRYSTAL_DATA_TYPE_STR)
};

#undef CRYSTAL_DATA_TYPE_STR

const char* dataTypeToString(DataType type) {
  return sDataTypeStrings[static_cast<int>(type)];
}

DataType stringToDataType(const char* str) {
  size_t n = std::size(sDataTypeStrings);
  for (size_t i = 0; i < n; ++i) {
    if (strcasecmp(str, sDataTypeStrings[i]) == 0) {
      return static_cast<DataType>(i);
    }
  }
  return DataType::UNKNOWN;
}

}  // namespace crystal
