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
  struct Meta {
    struct ElementMeta {
      DataType type;
      uint32_t count;
      uint32_t offset;
      OffsetPtr<Meta> subTuple;
    };

    template <class T, std::enable_if_t<!is_untyped_tuple_v<T>, int> = 0>
    void addType(uint32_t count = 1);
    template <class T, std::enable_if_t<is_untyped_tuple_v<T>, int> = 0>
    void addType(Meta* subTuple, uint32_t count = 1);

    std::vector<ElementMeta> metas;
    size_t size{1};
  };

  untyped_tuple() noexcept = delete;

  ~untyped_tuple() {
    if (offset_) {
      uint8_t* old = offset_.get();
      offset_ = nullptr;
      if (*old >> 7 == 0) {
        std::free(old);
      }
    }
  }

  explicit untyped_tuple(const Meta* meta) : meta_(meta) {
    reset();
  }
  untyped_tuple(const Meta* meta, void* buffer) : meta_(meta) {
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
    return *reinterpret_cast<T*>(offset_ + meta_->metas.at(i).offset);
  }
  template <class T>
  const T& at(size_t i) const {
    return *reinterpret_cast<const T*>(offset_ + meta_->metas.at(i).offset);
  }

  template <class T>
  T& get(size_t i) {
    return *reinterpret_cast<T*>(offset_ + meta_->metas[i].offset);
  }
  template <class T>
  const T& get(size_t i) const {
    return *reinterpret_cast<const T*>(offset_ + meta_->metas[i].offset);
  }

  size_t tuple_size() const noexcept {
    return meta_->metas.size();
  }
  size_t fixed_size() const noexcept {
    return meta_->size;
  }
  size_t element_buffer_size(size_t i) const noexcept;

  template <class F>
  void write(size_t size, F f) {
    uint8_t* p = reinterpret_cast<uint8_t*>(std::malloc(size + 1));
    f(p);
    if (offset_) {
      uint8_t* old = offset_.get();
      offset_ = p;
      if (*old >> 7 == 0) {
        std::free(old);
      }
    } else {
      offset_ = p;
    }
  }

  const Meta* meta() const { return meta_; }

  friend void serialize(
      const untyped_tuple& from, untyped_tuple& to, uint8_t* buffer);

 private:
  OffsetPtr<uint8_t> offset_;
  OffsetPtr<Meta> meta_;
};

template <class T, std::enable_if_t<!is_untyped_tuple_v<T>, int>>
void untyped_tuple::Meta::addType(uint32_t count) {
  metas.push_back(
      {
        DataTypeTraits<T>::value,
        count,
        size,
        nullptr
      });
  size += count == 0 ? sizeof(vector<T>) : sizeof(T) * count;
}

template <class T, std::enable_if_t<is_untyped_tuple_v<T>, int>>
void untyped_tuple::Meta::addType(Meta* subTuple, uint32_t count) {
  metas.push_back(
      {
        DataTypeTraits<T>::value,
        count,
        size,
        subTuple
      });
  size += count == 0 ? sizeof(vector<T>) : sizeof(T) * count;
}

} // namespace crystal
