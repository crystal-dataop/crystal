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

#include "crystal/record/containers/SerializationInUpdating.h"

namespace crystal {

size_t bufferSizeToUpdate(const untyped_tuple::meta& value) {
  size_t n = value.with_buffer_mask() ? 0 : value.fixed_size();
  for (auto& em : value) {
    if (em.submeta != nullptr) {
      n += bufferSizeToUpdate(untyped_tuple::meta{em.submeta});
    }
  }
  return n;
}

size_t bufferSizeToUpdate(const untyped_tuple& value) {
  size_t n = value.with_buffer_mask() ? 0 : value.fixed_size();
  for (size_t i = 0; i < value.size(); ++i) {
    n += value.element_buffer_size_to_update(i);
  }
  return n;
}

void serializeInUpdating(untyped_tuple::meta& value, void* buffer) {
  if (!value.with_buffer_mask()) {
    untyped_tuple::meta::head* buf =
      reinterpret_cast<untyped_tuple::meta::head*>(buffer);
    size_t n = value.fixed_size();
    std::memcpy(buf, value.offset.get(), n);
    buf->mask = 1;
    uint8_t* p = reinterpret_cast<uint8_t*>(buf) + n;
    for (size_t i = 0; i < value.size(); ++i) {
      if (value[i].submeta != nullptr) {
        untyped_tuple::meta& subfrom =
          *reinterpret_cast<untyped_tuple::meta*>(&value[i].submeta);
        untyped_tuple::meta& subto =
          *reinterpret_cast<untyped_tuple::meta*>(
              &untyped_tuple::meta{buf}[i].submeta);
        n = bufferSizeToUpdate(subfrom);
        serializeInUpdating(subfrom, p);
        subto.offset = subfrom.offset;
        subfrom.offset = nullptr;
        p += n;
      }
    }
    value.offset = buf;
  } else {
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    for (size_t i = 0; i < value.size(); ++i) {
      if (value[i].submeta != nullptr) {
        untyped_tuple::meta& subfrom =
          *reinterpret_cast<untyped_tuple::meta*>(&value[i].submeta);
        size_t n = bufferSizeToUpdate(subfrom);
        serializeInUpdating(subfrom, p);
        p += n;
      }
    }
  }
}

void serializeInUpdating(untyped_tuple& value, void* buffer) {
  if (!value.with_buffer_mask()) {
    uint8_t* old = value.offset_.get();
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
    std::memcpy(buf, old, 1);
    setMask(buf, true);
    uint8_t* p = buf + value.fixed_size();
    for (size_t i = 0; i < value.size(); ++i) {
      auto& em = value.meta_[i];
      switch (em.type) {
#define CASE(dt, t)                                                   \
        case DataType::dt: {                                          \
          if (em.count == 0) {                                        \
            using vt = vector<t>;                                     \
            vt& subfrom = *reinterpret_cast<vt*>(old + em.offset);    \
            vt& subto = *reinterpret_cast<vt*>(buf + em.offset);      \
            size_t n = bufferSizeToUpdate(subfrom);                   \
            serializeInUpdating(subfrom, p);                          \
            new (&subto) vt();                                        \
            new (&subto) vt(std::move(subfrom));                      \
            p += n;                                                   \
          } else {                                                    \
            for (uint32_t k = 0; k < em.count; ++k) {                 \
              t& subfrom = reinterpret_cast<t*>(old + em.offset)[k];  \
              t& subto = reinterpret_cast<t*>(buf + em.offset)[k];    \
              size_t n = bufferSizeToUpdate(subfrom);                 \
              serializeInUpdating(subfrom, p);                        \
              new (&subto) t();                                       \
              new (&subto) t(std::move(subfrom));                     \
              p += n;                                                 \
            }                                                         \
          }                                                           \
          break;                                                      \
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
    }
    value.set_buffer(buf);
  } else {
    uint8_t* old = value.offset_.get();
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    for (size_t i = 0; i < value.size(); ++i) {
      auto& em = value.meta_[i];
      switch (em.type) {
#define CASE(dt, t)                                                   \
        case DataType::dt: {                                          \
          if (em.count == 0) {                                        \
            using vt = vector<t>;                                     \
            vt& subfrom = *reinterpret_cast<vt*>(old + em.offset);    \
            size_t n = bufferSizeToUpdate(subfrom);                   \
            serializeInUpdating(subfrom, p);                          \
            p += n;                                                   \
          } else {                                                    \
            for (uint32_t k = 0; k < em.count; ++k) {                 \
              t& subfrom = reinterpret_cast<t*>(old + em.offset)[k];  \
              size_t n = bufferSizeToUpdate(subfrom);                 \
              serializeInUpdating(subfrom, p);                        \
              p += n;                                                 \
            }                                                         \
          }                                                           \
          break;                                                      \
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
    }
  }
}

} // namespace crystal
