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

#include "crystal/serializer/record/AllocMask.h"
#include "crystal/serializer/record/OffsetPtr.h"
#include "crystal/serializer/record/containers/Vector.h"
#include "crystal/type/DataType.h"

namespace crystal {

class untyped_tuple;

template <class T>
struct is_untyped_tuple : std::false_type {};

template <>
struct is_untyped_tuple<untyped_tuple> : std::true_type {};

template <class T>
inline constexpr auto is_untyped_tuple_v = is_untyped_tuple<T>::value;

class untyped_tuple {
 public:
  struct meta {
    struct element {
      DataType type;
      uint32_t count;
      uint32_t offset;
      OffsetPtr<meta> submeta;
    };

    ~meta() {
      if (offset_) {
        head* old = offset_.get();
        offset_ = nullptr;
        if (!old->mask) {
          std::free(old);
        }
      }
    }

    void resize(size_t n) {
      if (n == size()) {
        return;
      }
      if (n >> kNoMaskBitCount<uint32_t> > 0) {
        throw std::overflow_error("untyped_tuple::meta::resize");
      }
      head* p = reinterpret_cast<head*>(
          std::malloc(n * sizeof(element) + sizeof(head)));
      *reinterpret_cast<uint64_t*>(p) = 1;
      if (offset_) {
        head* old = offset_.get();
        offset_ = p;
        if (!old->mask) {
          std::free(old);
        }
      } else {
        offset_ = p;
      }
    }

    size_t size() const noexcept {
      return offset_ ? offset_->elem : 0;
    }
    size_t fixed_size() const noexcept {
      return offset_ ? offset_->size : 0;
    }

    element& operator[](size_t i) {
      return reinterpret_cast<element*>(offset_ + 1)[i];
    }
    const element& operator[](size_t i) const {
      return reinterpret_cast<const element*>(offset_ + 1)[i];
    }

    const element* begin() const noexcept {
      return offset_ ? reinterpret_cast<const element*>(offset_ + 1) : nullptr;
    }
    const element* end() const noexcept {
      return begin() + size();
    }

    template <class T, std::enable_if_t<!is_untyped_tuple_v<T>, int> = 0>
    void add_type(uint32_t count = 1) {
      add_type<T>(nullptr, count);
    }
    template <class T, std::enable_if_t<is_untyped_tuple_v<T>, int> = 0>
    void add_type(meta* submeta, uint32_t count = 1) {
      add_type<T>(submeta, count);
    }

    friend void serialize(const untyped_tuple::meta& from,
                          untyped_tuple::meta& to,
                          uint8_t* buffer);

   private:
    template <class T>
    void add_type(meta* submeta, uint32_t count) {
      if (offset_) {
        new (&operator[](offset_->elem)) element
            {
              DataTypeTraits<T>::value,
              count,
              offset_->size,
              submeta
            };
        offset_->size += count == 0 ? sizeof(vector<T>) : sizeof(T) * count;
      }
    }

    struct head {
      uint32_t mask : 1;
      uint32_t elem : 31;
      uint32_t size;
    };

    OffsetPtr<head> offset_;
  };

 public:
  untyped_tuple() noexcept = delete;

  ~untyped_tuple() {
    if (offset_) {
      uint8_t* old = offset_.get();
      offset_ = nullptr;
      if (!mask(*old)) {
        std::free(old);
      }
    }
  }

  explicit untyped_tuple(const meta* meta) : meta_(meta) {
    reset();
  }
  untyped_tuple(const meta* meta, void* buffer) : meta_(meta) {
    assign(buffer);
  }
  untyped_tuple(const untyped_tuple& other) {
    assign(other);
  }
  untyped_tuple(untyped_tuple&& other) noexcept {
    std::swap(offset_, other.offset_);
  }

  untyped_tuple& operator=(const untyped_tuple& other) {
    assign(other);
    return *this;
  }
  untyped_tuple& operator=(untyped_tuple&& other) noexcept {
    std::swap(offset_, other.offset_);
    return *this;
  }

  void reset();

  void assign(void* buffer) {
    offset_ = reinterpret_cast<uint8_t*>(buffer);
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
    return *reinterpret_cast<T*>(offset_ + (*meta_)[i].offset);
  }
  template <class T>
  const T& get(size_t i) const {
    return *reinterpret_cast<const T*>(offset_ + (*meta_)[i].offset);
  }

  size_t size() const noexcept {
    return meta_->size();
  }
  size_t fixed_size() const noexcept {
    return meta_->fixed_size();
  }
  size_t element_buffer_size(size_t i) const noexcept;

  template <class F>
  void write(size_t size, F f) {
    uint8_t* p = reinterpret_cast<uint8_t*>(std::malloc(size + 1));
    f(p);
    if (offset_) {
      uint8_t* old = offset_.get();
      offset_ = p;
      if (!mask(*old)) {
        std::free(old);
      }
    } else {
      offset_ = p;
    }
  }

  friend void serialize(
      const untyped_tuple& from, untyped_tuple& to, void* buffer);

 private:
  OffsetPtr<uint8_t> offset_;
  OffsetPtr<meta> meta_;
};

} // namespace crystal
