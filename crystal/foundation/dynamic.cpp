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

#include "crystal/foundation/dynamic.h"

#include <numeric>
#include <string>

#include "crystal/foundation/Hash.h"

namespace crystal {

//////////////////////////////////////////////////////////////////////

#define CRYSTAL_DYNAMIC_DEF_TYPEINFO(T) \
  constexpr const char* dynamic::TypeInfo<T>::name; \
  constexpr dynamic::Type dynamic::TypeInfo<T>::type; \
  //

CRYSTAL_DYNAMIC_DEF_TYPEINFO(std::nullptr_t)
CRYSTAL_DYNAMIC_DEF_TYPEINFO(bool)
CRYSTAL_DYNAMIC_DEF_TYPEINFO(std::string)
CRYSTAL_DYNAMIC_DEF_TYPEINFO(dynamic::Array)
CRYSTAL_DYNAMIC_DEF_TYPEINFO(double)
CRYSTAL_DYNAMIC_DEF_TYPEINFO(int64_t)
CRYSTAL_DYNAMIC_DEF_TYPEINFO(dynamic::ObjectImpl)

#undef CRYSTAL_DYNAMIC_DEF_TYPEINFO

const char* dynamic::typeName() const {
  return typeName(type_);
}

TypeError::TypeError(const std::string& expected, dynamic::Type actual)
    : std::runtime_error(toString(
          "TypeError: expected dynamic type `", expected, "', but had type `",
          dynamic::typeName(actual), "'")) {}

TypeError::TypeError(
    const std::string& expected,
    dynamic::Type actual1,
    dynamic::Type actual2)
    : std::runtime_error(toString(
          "TypeError: expected dynamic types `", expected, ", but had types `",
          dynamic::typeName(actual1), "' and `",
          dynamic::typeName(actual2), "'")) {}

TypeError::TypeError(const TypeError&) noexcept(
    std::is_nothrow_copy_constructible<std::runtime_error>::value) = default;
TypeError& TypeError::operator=(const TypeError&) noexcept(
    std::is_nothrow_copy_assignable<std::runtime_error>::value) = default;
TypeError::TypeError(TypeError&&) noexcept(
    std::is_nothrow_move_constructible<std::runtime_error>::value) = default;
TypeError& TypeError::operator=(TypeError&&) noexcept(
    std::is_nothrow_move_assignable<std::runtime_error>::value) = default;
TypeError::~TypeError() = default;

// This is a higher-order preprocessor macro to aid going from runtime
// types to the compile time type system.
#define CRYSTAL_DYNAMIC_APPLY(type, apply)  \
  do {                                      \
    switch ((type)) {                       \
      case NULLT:                           \
        apply(std::nullptr_t);              \
        break;                              \
      case ARRAY:                           \
        apply(Array);                       \
        break;                              \
      case BOOL:                            \
        apply(bool);                        \
        break;                              \
      case DOUBLE:                          \
        apply(double);                      \
        break;                              \
      case INT64:                           \
        apply(int64_t);                     \
        break;                              \
      case OBJECT:                          \
        apply(ObjectImpl);                  \
        break;                              \
      case STRING:                          \
        apply(std::string);                 \
        break;                              \
      default:                              \
        CRYSTAL_CHECK(0);                   \
        abort();                            \
    }                                       \
  } while (0)

bool dynamic::operator<(dynamic const& o) const {
  if (CRYSTAL_UNLIKELY(type_ == OBJECT || o.type_ == OBJECT)) {
    throw TypeError("object", type_);
  }
  if (type_ != o.type_) {
    return type_ < o.type_;
  }

#define CRYSTAL_X(T) return CompareOp<T>::comp(*getAddress<T>(),  \
                                           *o.getAddress<T>())
  CRYSTAL_DYNAMIC_APPLY(type_, CRYSTAL_X);
#undef CRYSTAL_X
}

bool dynamic::operator==(dynamic const& o) const {
  if (type() != o.type()) {
    if (isNumber() && o.isNumber()) {
      auto& integ = isInt() ? *this : o;
      auto& doubl = isInt() ? o     : *this;
      return integ.asInt() == doubl.asDouble();
    }
    return false;
  }

#define CRYSTAL_X(T) return *getAddress<T>() == *o.getAddress<T>();
  CRYSTAL_DYNAMIC_APPLY(type_, CRYSTAL_X);
#undef CRYSTAL_X
}

dynamic& dynamic::operator=(dynamic const& o) {
  if (&o != this) {
    if (type_ == o.type_) {
#define CRYSTAL_X(T) *getAddress<T>() = *o.getAddress<T>()
      CRYSTAL_DYNAMIC_APPLY(type_, CRYSTAL_X);
#undef CRYSTAL_X
    } else {
      destroy();
#define CRYSTAL_X(T) new (getAddress<T>()) T(*o.getAddress<T>())
      CRYSTAL_DYNAMIC_APPLY(o.type_, CRYSTAL_X);
#undef CRYSTAL_X
      type_ = o.type_;
    }
  }
  return *this;
}

dynamic& dynamic::operator=(dynamic&& o) noexcept {
  if (&o != this) {
    if (type_ == o.type_) {
#define CRYSTAL_X(T) *getAddress<T>() = std::move(*o.getAddress<T>())
      CRYSTAL_DYNAMIC_APPLY(type_, CRYSTAL_X);
#undef CRYSTAL_X
    } else {
      destroy();
#define CRYSTAL_X(T) new (getAddress<T>()) T(std::move(*o.getAddress<T>()))
      CRYSTAL_DYNAMIC_APPLY(o.type_, CRYSTAL_X);
#undef CRYSTAL_X
      type_ = o.type_;
    }
  }
  return *this;
}

dynamic& dynamic::operator[](dynamic const& k) & {
  if (!isObject() && !isArray()) {
    throw TypeError("object/array", type());
  }
  if (isArray()) {
    return at(k);
  }
  auto& obj = get<ObjectImpl>();
  auto ret = obj.insert({k, nullptr});
  return ret.first->second;
}

dynamic dynamic::getDefault(const dynamic& k, const dynamic& v) const& {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  return it == obj.end() ? v : it->second;
}

dynamic dynamic::getDefault(const dynamic& k, dynamic&& v) const& {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  // Avoid clang bug with ternary
  if (it == obj.end()) {
    return std::move(v);
  } else {
    return it->second;
  }
}

dynamic dynamic::getDefault(const dynamic& k, const dynamic& v) && {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  // Avoid clang bug with ternary
  if (it == obj.end()) {
    return v;
  } else {
    return std::move(it->second);
  }
}

dynamic dynamic::getDefault(const dynamic& k, dynamic&& v) && {
  auto& obj = get<ObjectImpl>();
  auto it = obj.find(k);
  return std::move(it == obj.end() ? v : it->second);
}

const dynamic* dynamic::get_ptr(dynamic const& idx) const& {
  if (auto* parray = get_nothrow<Array>()) {
    if (!idx.isInt()) {
      throw TypeError("int64", idx.type());
    }
    if (idx < 0 || idx >= parray->size()) {
      return nullptr;
    }
    return &(*parray)[size_t(idx.asInt())];
  } else if (auto* pobject = get_nothrow<ObjectImpl>()) {
    auto it = pobject->find(idx);
    if (it == pobject->end()) {
      return nullptr;
    }
    return &it->second;
  } else {
    throw TypeError("object/array", type());
  }
}

[[noreturn]] static void throwOutOfRangeAtMissingKey(dynamic const& idx) {
  throw std::out_of_range(toString(
      "couldn't find key ", idx.asString(), " in dynamic object"));
}

dynamic const& dynamic::at(dynamic const& idx) const& {
  if (auto* parray = get_nothrow<Array>()) {
    if (!idx.isInt()) {
      throw TypeError("int64", idx.type());
    }
    if (idx < 0 || idx >= parray->size()) {
      throw std::out_of_range("out of range in dynamic array");
    }
    return (*parray)[size_t(idx.asInt())];
  } else if (auto* pobject = get_nothrow<ObjectImpl>()) {
    auto it = pobject->find(idx);
    if (it == pobject->end()) {
      throwOutOfRangeAtMissingKey(idx);
    }
    return it->second;
  } else {
    throw TypeError("object/array", type());
  }
}

std::size_t dynamic::size() const {
  if (auto* ar = get_nothrow<Array>()) {
    return ar->size();
  }
  if (auto* obj = get_nothrow<ObjectImpl>()) {
    return obj->size();
  }
  if (auto* str = get_nothrow<std::string>()) {
    return str->size();
  }
  throw TypeError("array/object/string", type());
}

dynamic::iterator dynamic::erase(const_iterator first, const_iterator last) {
  auto& arr = get<Array>();
  return get<Array>().erase(
    arr.begin() + (first - arr.begin()),
    arr.begin() + (last - arr.begin()));
}

std::size_t dynamic::hash() const {
  switch (type()) {
  case NULLT:
    return 0xBAAAAAAD;
  case OBJECT:
  {
    // Accumulate using addition instead of using hash_range (as in the ARRAY
    // case), as we need a commutative hash operation since unordered_map's
    // iteration order is unspecified.
    return std::accumulate(
        items().begin(),
        items().end(),
        size_t{0x0B1EC7},
        [&](auto acc, auto item) {
          return acc + hash_128_to_64(item.first.hash(), item.second.hash());
        });
    }
  case ARRAY:
    return hash_range(begin(), end());
  case INT64:
    return std::hash<int64_t>()(getInt());
  case DOUBLE:
    return std::hash<double>()(getDouble());
  case BOOL:
    return std::hash<bool>()(getBool());
  case STRING:
    return std::hash<std::string>()(getString());
  }
}

char const* dynamic::typeName(Type t) {
#define CRYSTAL_X(T) return TypeInfo<T>::name
  CRYSTAL_DYNAMIC_APPLY(t, CRYSTAL_X);
#undef CRYSTAL_X
}

void dynamic::destroy() noexcept {
  // This short-circuit speeds up some microbenchmarks.
  if (type_ == NULLT) {
    return;
  }

#define CRYSTAL_X(T) detail::Destroy::destroy(getAddress<T>())
  CRYSTAL_DYNAMIC_APPLY(type_, CRYSTAL_X);
#undef CRYSTAL_X
  type_ = NULLT;
  u_.nul = nullptr;
}

dynamic dynamic::merge_diff(const dynamic& source, const dynamic& target) {
  if (!source.isObject() || source.type() != target.type()) {
    return target;
  }

  dynamic diff = object;

  // added/modified keys
  for (const auto& pair : target.items()) {
    auto it = source.find(pair.first);
    if (it == source.items().end()) {
      diff[pair.first] = pair.second;
    } else {
      diff[pair.first] = merge_diff(source[pair.first], target[pair.first]);
    }
  }

  // removed keys
  for (const auto& pair : source.items()) {
    auto it = target.find(pair.first);
    if (it == target.items().end()) {
      diff[pair.first] = nullptr;
    }
  }

  return diff;
}

//////////////////////////////////////////////////////////////////////

} // namespace crystal
