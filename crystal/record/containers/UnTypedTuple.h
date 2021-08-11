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

#include <cstdlib>
#include <type_traits>

#include "crystal/record/AllocMask.h"
#include "crystal/record/OffsetPtr.h"
#include "crystal/record/containers/Vector.h"
#include "crystal/type/DataType.h"

namespace crystal {

class untyped_tuple;

template <class T>
struct is_untyped_tuple : std::false_type {};

template <>
struct is_untyped_tuple<untyped_tuple> : std::true_type {};

template <class T>
inline constexpr bool is_untyped_tuple_v = is_untyped_tuple<T>::value;

template <>
struct DataTypeTraits<untyped_tuple> {
  enum {
    value = static_cast<int>(DataType::TUPLE)
  };
};

class untyped_tuple {
 public:
  struct meta {
    struct head {
      uint32_t mask : 1;
      uint32_t elem : 31;
      uint32_t size;
    };
    struct element {
      DataType type;
      uint32_t count;
      uint32_t offset;
      OffsetPtr<head> submeta;
    };

    OffsetPtr<head> offset;

    void release() {
      if (offset) {
        head* old = offset.get();
        offset = nullptr;
        if (!old->mask) {
          std::free(old);
        }
      }
    }

    void reserve(size_t n) {
      if (n == size()) {
        return;
      }
      if (n >> kNoMaskBitCount<uint32_t> > 0) {
        throw std::overflow_error("meta::reserve");
      }
      head* p = reinterpret_cast<head*>(
          std::malloc(n * sizeof(element) + sizeof(head)));
      p->mask = 0;
      p->elem = 0;
      p->size = 1;
      if (offset) {
        throw std::runtime_error("meta::reserve");
      }
      offset = p;
    }

    size_t size() const noexcept {
      return offset ? offset->elem : 0;
    }
    size_t fixed_size() const noexcept {
      return offset ? offset->elem * sizeof(untyped_tuple::meta::element)
                      + sizeof(untyped_tuple::meta::head) : 0;
    }
    size_t tuple_fixed_size() const noexcept {
      return offset ? offset->size : 0;
    }

    element& operator[](size_t i) {
      return reinterpret_cast<element*>(offset + 1)[i];
    }
    const element& operator[](size_t i) const {
      return reinterpret_cast<const element*>(offset + 1)[i];
    }

    const element* begin() const noexcept {
      return offset ? reinterpret_cast<const element*>(offset + 1) : nullptr;
    }
    const element* end() const noexcept {
      return begin() + size();
    }

    template <class T>
    void add_type(const meta& submeta, uint32_t count) {
      if (offset) {
        new (&operator[](offset->elem)) element
            {
              static_cast<DataType>(DataTypeTraits<T>::value),
              count,
              offset->size,
              submeta.offset
            };
        offset->elem++;
        offset->size += count == 0 ? sizeof(vector<T>) : sizeof(T) * count;
      }
    }

    bool with_buffer_mask() const noexcept {
      return offset && offset->mask;
    }
  };

 public:
  untyped_tuple() noexcept = default;

  ~untyped_tuple() {
    set_buffer(nullptr);
  }

  explicit untyped_tuple(const meta& meta) : meta_(meta) {
    reset();
  }
  untyped_tuple(const meta& meta, void* buffer) : meta_(meta) {
    set_buffer(buffer);
  }
  untyped_tuple(const untyped_tuple& other) : meta_(other.meta_) {
    assign(other);
  }
  untyped_tuple(untyped_tuple&& other) noexcept : meta_(other.meta_) {
    std::swap(offset_, other.offset_);
  }

  untyped_tuple& operator=(const untyped_tuple& other) {
    meta_ = other.meta_;
    assign(other);
    return *this;
  }
  untyped_tuple& operator=(untyped_tuple&& other) noexcept {
    meta_ = other.meta_;
    std::swap(offset_, other.offset_);
    return *this;
  }

  void reset();

  void reset(const meta& meta) {
    meta_ = meta;
    reset();
  }

  void assign(const untyped_tuple& other);

  template <class T>
  T& at(size_t i) {
    if (i >= size()) {
      throw std::out_of_range("untyped_tuple::at");
    }
    return get<T>(i);
  }
  template <class T>
  const T& at(size_t i) const {
    if (i >= size()) {
      throw std::out_of_range("untyped_tuple::at");
    }
    return get<T>(i);
  }

  template <class T>
  T& get(size_t i) {
    return *reinterpret_cast<T*>(offset_ + meta_[i].offset);
  }
  template <class T>
  const T& get(size_t i) const {
    return *reinterpret_cast<const T*>(offset_ + meta_[i].offset);
  }

  size_t size() const noexcept {
    return meta_.size();
  }
  size_t fixed_size() const noexcept {
    return meta_.tuple_fixed_size();
  }
  size_t element_buffer_size(size_t i) const noexcept;
  size_t element_buffer_size_to_update(size_t i) const noexcept;

  template <class F>
  void write(size_t size, F f) {
    uint8_t* p = reinterpret_cast<uint8_t*>(std::malloc(size + 1));
    setMask(p, false);
    f(p);
    set_buffer(p);
  }

  void set_buffer(void* buffer);

  bool with_buffer_mask() const noexcept {
    return offset_ && getMask(offset_);
  }

  friend void serialize(const untyped_tuple& from,
                        untyped_tuple& to,
                        void* buffer);
  friend void serializeInUpdating(untyped_tuple& value, void* buffer);

 private:
  OffsetPtr<uint8_t> offset_;

 public:
  meta meta_;
};

} // namespace crystal
