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

#include "crystal/serializer/record/FieldMeta.h"
#include "crystal/storage/table/Table.h"

namespace crystal {

#define CRYSTAL_FIELD_INFO_TYPE_GEN(x)  \
  x(UnUsed),                            \
  x(Payload),                           \
  x(Value),                             \
  x(PayloadAndValue),                   \
  x(Related)

struct FieldInfo {
#define CRYSTAL_FIELD_INFO_TYPE_ENUM(type) k##type

  enum {
    CRYSTAL_FIELD_INFO_TYPE_GEN(CRYSTAL_FIELD_INFO_TYPE_ENUM)
  };

#undef CRYSTAL_FIELD_INFO_TYPE_ENUM

  struct Related {
    int no{-1};
    int ref{-1};
    Table* table{nullptr};
  };

  FieldMeta meta;
  int type;
  IndexNoSet indexNo;
  Related related;
};

const char* fieldInfoTypeToString(int type);

} // namespace crystal
