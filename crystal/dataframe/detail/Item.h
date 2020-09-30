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

#include <optional>

#include "crystal/foundation/dynamic.h"
#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {

struct Item {
  template <class T>
  std::optional<T> get() const {
    if (item_.isNull()) {
      return std::nullopt;
    }
    T value;
    decode(item_, value);
    return value;
  }

  template <class T>
  void set(const T& value) {
    item_ = encode(value);
  }

 private:
  dynamic item_;
};

} // namespace crystal
