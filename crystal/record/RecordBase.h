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

#include "crystal/record/containers/Array.h"
#include "crystal/record/containers/String.h"
#include "crystal/record/containers/Tuple.h"
#include "crystal/record/containers/Vector.h"

namespace crystal {

class RecordBase {
 public:
  RecordBase(const untyped_tuple::meta& meta, void* buffer)
      : untyped_tuple_(meta, buffer) {}

  void setBuffer(void* buffer) {
    untyped_tuple_.set_buffer(buffer);
  }

  template <class T>
  T& get(size_t i) {
    return untyped_tuple_.get<T>(i);
  }
  template <class T>
  const T& get(size_t i) const {
    return untyped_tuple_.get<T>(i);
  }

  untyped_tuple& untypedTuple() {
    return untyped_tuple_;
  }
  const untyped_tuple& untypedTuple() const {
    return untyped_tuple_;
  }

 private:
  untyped_tuple untyped_tuple_;
};

} // namespace crystal
