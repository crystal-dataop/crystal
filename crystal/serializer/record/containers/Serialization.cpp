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

#include "crystal/serializer/record/containers/Serialization.h"

namespace crystal {

size_t bufferSize(const untyped_tuple::meta& value) {
  size_t n = value.size() * sizeof(untyped_tuple::meta::element)
           + sizeof(uint64_t);
  for (auto& em : value) {
    n += bufferSize(untyped_tuple::meta{em.submeta});
  }
  return n;
}

size_t bufferSize(const untyped_tuple& value) {
  size_t n = value.fixed_size();
  for (size_t i = 0; i < value.size(); ++i) {
    n += value.element_buffer_size(i);
  }
  return n;
}

void serialize(const untyped_tuple::meta& from,
               untyped_tuple::meta& to,
               void* buffer) {
}

void serialize(const untyped_tuple& from, untyped_tuple& to, void* buffer) {
  uint8_t* old = from.offset_.get();
  uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
  size_t n = from.fixed_size();
  std::memcpy(buf, old, n);
  setMask(buf, true);
  uint8_t* p = buf;
  p += n;
  for (size_t i = 0; i < from.size(); ++i) {
    auto& em = from.meta_[i];
    switch (em.type) {
#define CASE(dt, t)                                       \
      case DataType::dt:                                  \
        serialize(*reinterpret_cast<t*>(old + em.offset), \
                  *reinterpret_cast<t*>(buf + em.offset), \
                  p);                                     \
        break;

      CASE(BOOL, bool)
      CASE(INT8, int8_t)
      CASE(INT16, int16_t)
      CASE(INT32, int32_t)
      CASE(INT64, int64_t)
      CASE(UINT8, uint8_t)
      CASE(UINT16, uint16_t)
      CASE(UINT32, uint32_t)
      CASE(UINT64, uint64_t)
      CASE(FLOAT, float)
      CASE(DOUBLE, double)
      CASE(STRING, string)
      CASE(TUPLE, untyped_tuple)
      default: break;

#undef CASE
    }
    p += from.element_buffer_size(i);
  }
  to.meta_ = from.meta_;
  to.offset_ = buf;
}

} // namespace crystal
