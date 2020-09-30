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

#include "crystal/storage/table/FieldInfo.h"

namespace crystal {

#define CRYSTAL_FIELD_INFO_TYPE_STR(type) #type

namespace {

static const char* sFieldInfoTypeStrings[] = {
  CRYSTAL_FIELD_INFO_TYPE_GEN(CRYSTAL_FIELD_INFO_TYPE_STR)
};

}

const char* fieldInfoTypeToString(int type) {
  return sFieldInfoTypeStrings[static_cast<unsigned>(type)];
}

} // namespace crystal
