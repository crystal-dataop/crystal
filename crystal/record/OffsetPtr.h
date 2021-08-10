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

#include <cinttypes>
#include <limits>

namespace crystal {

constexpr auto const NULLPTR = std::numeric_limits<intptr_t>::min();

template <class T, class Enable = void>
struct OffsetPtr {
  OffsetPtr() noexcept = default;
  ~OffsetPtr() noexcept = default;

  OffsetPtr(const T* p) noexcept : offset_(ptr2Offset(p)) {}
  OffsetPtr(std::nullptr_t) noexcept : offset_(NULLPTR) {}

  OffsetPtr& operator=(const T* p) noexcept {
    offset_ = ptr2Offset(p);
    return *this;
  }
  OffsetPtr& operator=(std::nullptr_t) noexcept {
    offset_ = NULLPTR;
    return *this;
  }

  OffsetPtr(const OffsetPtr& o) noexcept : offset_(ptr2Offset(o.get())) {}
  OffsetPtr(OffsetPtr&& o) noexcept : offset_(ptr2Offset(o.get())) {}

  OffsetPtr& operator=(const OffsetPtr& o) noexcept {
    offset_ = ptr2Offset(o.get());
    return *this;
  }
  OffsetPtr& operator=(OffsetPtr&& o) noexcept {
    offset_ = ptr2Offset(o.get());
    return *this;
  }

  intptr_t ptr2Offset(const T* p) const noexcept {
    return p == nullptr ? NULLPTR : reinterpret_cast<intptr_t>(p) -
                                    reinterpret_cast<intptr_t>(this);
  }

  explicit operator bool() const noexcept { return offset_ != NULLPTR; }
  explicit operator void*() const noexcept { return get(); }
  explicit operator const void*() const noexcept { return get(); }

  operator T*() const noexcept { return get(); }
  T& operator*() const noexcept { return *get(); }
  T* operator->() const noexcept { return get(); }
  T& operator[](size_t i) const noexcept { return get()[i]; }

  T* get() const noexcept {
    return offset_ == NULLPTR
      ? nullptr : reinterpret_cast<T*>(
                    reinterpret_cast<intptr_t>(this) + offset_);
  }

  T* operator+(size_t i) const noexcept {
    return get() + i;
  }
  T* operator-(size_t i) const noexcept {
    return get() - i;
  }
  OffsetPtr& operator++() noexcept {
    offset_ = ptr2Offset(get() + 1);
    return *this;
  }
  OffsetPtr& operator--() noexcept {
    offset_ = ptr2Offset(get() - 1);
    return *this;
  }
  OffsetPtr operator++(int) const noexcept {
    return OffsetPtr(get() + 1);
  }
  OffsetPtr operator--(int) const noexcept {
    return OffsetPtr(get() - 1);
  }

  intptr_t offset_{NULLPTR};
};

template <class T>
struct OffsetPtr<T, std::enable_if_t<std::is_same_v<void, T>>> {
  OffsetPtr() noexcept = default;
  ~OffsetPtr() noexcept = default;

  OffsetPtr(const T* p) noexcept : offset_(ptr2Offset(p)) {}
  OffsetPtr(std::nullptr_t) noexcept : offset_(NULLPTR) {}

  OffsetPtr& operator=(const T* p) noexcept {
    offset_ = ptr2Offset(p);
    return *this;
  }
  OffsetPtr& operator=(std::nullptr_t) noexcept {
    offset_ = NULLPTR;
    return *this;
  }

  OffsetPtr(const OffsetPtr& o) noexcept : offset_(ptr2Offset(o.get())) {}
  OffsetPtr(OffsetPtr&& o) noexcept : offset_(ptr2Offset(o.get())) {}

  OffsetPtr& operator=(const OffsetPtr& o) noexcept {
    offset_ = ptr2Offset(o.get());
    return *this;
  }
  OffsetPtr& operator=(OffsetPtr&& o) noexcept {
    offset_ = ptr2Offset(o.get());
    return *this;
  }

  intptr_t ptr2Offset(const T* p) const noexcept {
    return p == nullptr ? NULLPTR : reinterpret_cast<intptr_t>(p) -
                                    reinterpret_cast<intptr_t>(this);
  }

  explicit operator bool() const noexcept { return offset_ != NULLPTR; }
  explicit operator void*() const noexcept { return get(); }
  explicit operator const void*() const noexcept { return get(); }

  T* get() const noexcept {
    return offset_ == NULLPTR
      ? nullptr : reinterpret_cast<T*>(
                    reinterpret_cast<intptr_t>(this) + offset_);
  }

  friend bool operator==(std::nullptr_t, const OffsetPtr& o) noexcept {
    return o.offset_ == NULLPTR;
  }

  friend bool operator==(const OffsetPtr& o, std::nullptr_t) noexcept {
    return o.offset_ == NULLPTR;
  }

  friend bool operator!=(std::nullptr_t, const OffsetPtr& o) noexcept {
    return o.offset_ != NULLPTR;
  }

  friend bool operator!=(const OffsetPtr& o, std::nullptr_t) noexcept {
    return o.offset_ != NULLPTR;
  }

  intptr_t offset_{NULLPTR};
};

} // namespace crystal
