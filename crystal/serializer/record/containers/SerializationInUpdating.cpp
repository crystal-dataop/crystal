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

#include "crystal/serializer/record/containers/SerializationInUpdating.h"

namespace crystal {

size_t bufferSizeToUpdate(const untyped_tuple::meta& value) {
  size_t n = value.with_buffer_mask()
    ? 0 : value.size() * sizeof(untyped_tuple::meta::element)
        + sizeof(uint64_t);
  for (auto& em : value) {
    n += bufferSizeToUpdate(untyped_tuple::meta{em.submeta});
  }
  return n;
}

size_t bufferSizeToUpdate(const untyped_tuple& value) {
  size_t n = value.with_buffer_mask() ? 0 : value.fixed_size();
  for (size_t i = 0; i < value.size(); ++i) {
    n += value.element_buffer_size(i);
  }
  return n;
}

void serializeInUpdating(untyped_tuple::meta& value, void* buffer) {
}

void serializeInUpdating(untyped_tuple& value, void* buffer) {
  if (!value.with_buffer_mask()) {
    uint8_t* old = value.offset_.get();
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
    size_t n = value.fixed_size();
    std::memcpy(buf, old, n);
    setMask(buf);
    uint8_t* p = buf;
    p += n;
    for (size_t i = 0; i < value.size(); ++i) {
      auto& em = value.meta_[i];
      switch (em.type) {
#define CASE(dt, t)                                         \
        case DataType::dt: {                                \
          t& from = *reinterpret_cast<t*>(old + em.offset); \
          t& to = *reinterpret_cast<t*>(buf + em.offset);   \
          syncOffset(from, to);                             \
          serializeInUpdating(to, p);                       \
          break;                                            \
        }

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
      p += value.element_buffer_size_to_update(i);
    }
    value.set_buffer(buf);
  } else {
    uint8_t* old = value.offset_.get();
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    for (size_t i = 0; i < value.size(); ++i) {
      auto& em = value.meta_[i];
      switch (em.type) {
#define CASE(dt, t)                                         \
        case DataType::dt: {                                \
          t& from = *reinterpret_cast<t*>(old + em.offset); \
          serializeInUpdating(from, p);                     \
          break;                                            \
        }

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
      p += value.element_buffer_size_to_update(i);
    }
  }
}

} // namespace crystal
