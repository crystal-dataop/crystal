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
#include "crystal/record/containers/SerializationInUpdating.h"
#include "crystal/record/containers/String.h"
#include "crystal/record/containers/UnTypedTuple.h"

namespace crystal {

template <class T>
size_t bsize(uint8_t* dst, const untyped_tuple::meta::element& em,
             bool update = false) {
  if (em.count == 0) {
    return update ? bufferSizeToUpdate(*reinterpret_cast<vector<T>*>(dst))
                  : bufferSize(*reinterpret_cast<vector<T>*>(dst));
  } else {
    size_t n = 0;
    for (uint32_t i = 0; i < em.count; ++i) {
      n += update ? bufferSizeToUpdate(*(reinterpret_cast<T*>(dst) + i))
                  : bufferSize(*(reinterpret_cast<T*>(dst) + i));
    }
    return n;
  }
}

template <class T>
void place(uint8_t* dst, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    new (dst) vector<T>();
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      new (reinterpret_cast<T*>(dst) + i) T();
    }
  }
}

template <>
void place<untyped_tuple>(
    uint8_t* dst, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    new (dst) vector<untyped_tuple>();
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      new (reinterpret_cast<untyped_tuple*>(dst) + i)
        untyped_tuple(untyped_tuple::meta{em.submeta});
    }
  }
}

template <class T>
void place(uint8_t* dst, uint8_t* src, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    new (dst) vector<T>(*reinterpret_cast<vector<T>*>(src));
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      new (reinterpret_cast<T*>(dst) + i) T(*(reinterpret_cast<T*>(src) + i));
    }
  }
}

template <class T>
void init(uint8_t* dst, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    *reinterpret_cast<vector<T>*>(dst) = vector<T>();
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      *(reinterpret_cast<T*>(dst) + i) = T();
    }
  }
}

template <>
void init<untyped_tuple>(
    uint8_t* dst, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    *reinterpret_cast<vector<untyped_tuple>*>(dst) = vector<untyped_tuple>();
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      *(reinterpret_cast<untyped_tuple*>(dst) + i) =
        untyped_tuple(untyped_tuple::meta{em.submeta});
    }
  }
}

template <class T>
void release(uint8_t* dst, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    reinterpret_cast<vector<T>*>(dst)->~vector<T>();
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      (reinterpret_cast<T*>(dst) + i)->~T();
    }
  }
}

template <class T>
void copy(uint8_t* dst, uint8_t* src, const untyped_tuple::meta::element& em) {
  if (em.count == 0) {
    *reinterpret_cast<vector<T>*>(dst) = *reinterpret_cast<vector<T>*>(src);
  } else {
    for (uint32_t i = 0; i < em.count; ++i) {
      *(reinterpret_cast<T*>(dst) + i) = *(reinterpret_cast<T*>(src) + i);
    }
  }
}

void untyped_tuple::reset() {
  if (offset_) {
    for (auto& em : meta_) {
      uint8_t* e = offset_ + em.offset;
      switch (em.type) {
#define CASE(dt, t) \
        case DataType::dt: init<t>(e, em); break;

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
  } else {
    write(fixed_size(), [&](uint8_t* p) {
      for (auto& em : meta_) {
        uint8_t* e = p + em.offset;
        switch (em.type) {
#define CASE(dt, t) \
          case DataType::dt: place<t>(e, em); break;

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
    });
  }
}

void untyped_tuple::assign(const untyped_tuple& other) {
  if (offset_) {
    for (auto& em : meta_) {
      uint8_t* e = offset_ + em.offset;
      uint8_t* o = other.offset_ + em.offset;
      switch (em.type) {
#define CASE(dt, t) \
        case DataType::dt: copy<t>(e, o, em); break;

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
  } else {
    write(fixed_size(), [&](uint8_t* p) {
      for (auto& em : meta_) {
        uint8_t* e = p + em.offset;
        uint8_t* o = other.offset_ + em.offset;
        switch (em.type) {
#define CASE(dt, t) \
          case DataType::dt: place<t>(e, o, em); break;

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
    });
  }
}

size_t untyped_tuple::element_buffer_size(size_t i) const noexcept {
  if (offset_) {
    auto& em = meta_[i];
    uint8_t* e = offset_ + em.offset;
    switch (em.type) {
#define CASE(dt, t) \
      case DataType::dt: return bsize<t>(e, em);

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
  return 0;
}

size_t untyped_tuple::element_buffer_size_to_update(size_t i) const noexcept {
  if (offset_) {
    auto& em = meta_[i];
    uint8_t* e = offset_ + em.offset;
    switch (em.type) {
#define CASE(dt, t) \
      case DataType::dt: return bsize<t>(e, em, true);

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
  return 0;
}

void untyped_tuple::set_buffer(void* buffer) {
  uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
  if (offset_) {
    uint8_t* old = offset_.get();
    offset_ = p;
    for (auto& em : meta_) {
      uint8_t* e = old + em.offset;
      switch (em.type) {
#define CASE(dt, t) \
        case DataType::dt: release<t>(e, em); break;

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
    if (!getMask(old)) {
      std::free(old);
    }
  } else {
    offset_ = p;
  }
}

} // namespace crystal
