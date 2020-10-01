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

#include <string>

#include "crystal/foundation/ApplyMacro.h"

namespace crystal {

class DataView;

struct FieldIndexMeta {
  class Meta {
   public:
    explicit Meta(const std::string& name) : name_(name) {}

    size_t getIndex(const DataView& view);

    const std::string& name() const {
      return name_;
    }

   private:
    size_t index_{size_t(-1)};
    std::string name_;
  };
};

typedef FieldIndexMeta::Meta FieldKey;

#define CRYSTAL_FIELD_INDEX_FIELD(field) Meta field{#field};

#define CRYSTAL_FIELD_INDEX_META(meta, ...)                 \
  struct meta : public crystal::FieldIndexMeta {            \
    CRYSTAL_APPLYXn(CRYSTAL_FIELD_INDEX_FIELD, __VA_ARGS__) \
  }

} // namespace crystal
