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

#include "crystal/record/containers/Serialization.h"

namespace crystal {

size_t bufferSize(const untyped_tuple::meta& value) {
  size_t n = value.fixed_size();
  for (auto& em : value) {
    if (em.submeta != nullptr) {
      n += bufferSize(untyped_tuple::meta{em.submeta});
    }
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
  untyped_tuple::meta::head* buf =
    reinterpret_cast<untyped_tuple::meta::head*>(buffer);
  size_t n = from.fixed_size();
  std::memcpy(buf, from.offset.get(), n);
  buf->mask = 1;
  uint8_t* p = reinterpret_cast<uint8_t*>(buf) + n;
  for (size_t i = 0; i < from.size(); ++i) {
    if (from[i].submeta != nullptr) {
      const untyped_tuple::meta& subfrom =
        *reinterpret_cast<const untyped_tuple::meta*>(&from[i].submeta);
      untyped_tuple::meta& subto =
        *reinterpret_cast<untyped_tuple::meta*>(
            &untyped_tuple::meta{buf}[i].submeta);
      n = bufferSize(subfrom);
      serialize(subfrom, subto, p);
      p += n;
    }
  }
  to.offset = buf;
}

void serialize(const untyped_tuple& from, untyped_tuple& to, void* buffer) {
  uint8_t* old = from.offset_.get();
  uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
  std::memcpy(buf, old, 1);
  setMask(buf, true);
  uint8_t* p = buf + from.fixed_size();
  for (size_t i = 0; i < from.size(); ++i) {
    auto& em = from.meta_[i];
    switch (em.type) {
#define CASE(dt, t)                                                 \
      case DataType::dt: {                                          \
        if (em.count == 0) {                                        \
          using vt = vector<t>;                                     \
          vt& subfrom = *reinterpret_cast<vt*>(old + em.offset);    \
          vt& subto = *reinterpret_cast<vt*>(buf + em.offset);      \
          size_t n = bufferSize(subfrom);                           \
          new (&subto) vt();                                        \
          serialize(subfrom, subto, p);                             \
          p += n;                                                   \
        } else {                                                    \
          for (uint32_t k = 0; k < em.count; ++k) {                 \
            t& subfrom = reinterpret_cast<t*>(old + em.offset)[k];  \
            t& subto = reinterpret_cast<t*>(buf + em.offset)[k];    \
            size_t n = bufferSize(subfrom);                         \
            new (&subto) t();                                       \
            serialize(subfrom, subto, p);                           \
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
  to.set_buffer(buf);
  to.meta_ = from.meta_;
}

} // namespace crystal
