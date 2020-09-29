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

#include "crystal/strategy/DefaultStrategy.h"

#include <string_view>

#include "crystal/strategy/Hash.h"

namespace crystal {

std::vector<uint64_t> DefaultStrategy::getIndexKeys(const Record& record) {
  std::vector<uint64_t> keys;
  switch (keyMeta_.type()) {
#define CONV(type, enum_type)                                   \
    case DataType::enum_type: {                                 \
      if (keyMeta_.isArray()) {                                 \
        Array<type> a = record.get<Array<type>>(keyMeta_);      \
        for (size_t i = 0; i < a.size(); i++) {                 \
          keys.push_back(hashToken(a.get(i)));                  \
        }                                                       \
      } else {                                                  \
        keys.push_back(hashToken(record.get<type>(keyMeta_)));  \
      }                                                         \
      break;                                                    \
    }

    CONV(int8_t, INT8);
    CONV(int16_t, INT16);
    CONV(int32_t, INT32);
    CONV(int64_t, INT64);
    CONV(uint8_t, UINT8);
    CONV(uint16_t, UINT16);
    CONV(uint32_t, UINT32);
    CONV(uint64_t, UINT64);
    CONV(std::string_view, STRING);

#undef CONV

    default:
      CRYSTAL_LOG(ERROR) << "unsupport key type: "
          << dataTypeToString(keyMeta_.type());
      break;
  }
  return keys;
}

} // namespace crystal
