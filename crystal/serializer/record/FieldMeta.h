/*
 * Copyright 2020 Yeolar
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

#include <string>

#include "crystal/foundation/Exception.h"
#include "crystal/foundation/json.h"
#include "crystal/type/DataType.h"
#include "crystal/type/TypeTraits.h"

namespace crystal {

class FieldMeta {
 public:
  FieldMeta();
  FieldMeta(const std::string& name,
            int tag,
            DataType type,
            size_t bits,
            size_t count,
            const dynamic& dflt = nullptr);

  virtual ~FieldMeta() {}

  const std::string& name() const;
  int tag() const;
  DataType type() const;
  size_t bits() const;
  size_t count() const;

  bool isCompact() const;

  bool isArray() const;
  bool isFixArray() const;
  bool isVarArray() const;

  template <class T>
  T dflt() const;

  dynamic toDynamic() const;
  std::string toString() const;

 private:
  template <class T, class Enable = void>
  struct GetDefaultImpl;

  std::string name_;
  int tag_;
  DataType type_;
  size_t bits_;
  size_t count_;
  bool compact_;
  dynamic default_;
};

//////////////////////////////////////////////////////////////////////

inline const std::string& FieldMeta::name() const {
  return name_;
}

inline int FieldMeta::tag() const {
  return tag_;
}

inline DataType FieldMeta::type() const {
  return type_;
}

inline size_t FieldMeta::bits() const {
  return bits_;
}

inline size_t FieldMeta::count() const {
  return count_;
}

inline bool FieldMeta::isCompact() const {
  return compact_;
}

inline bool FieldMeta::isArray() const {
  return count_ != 1;
}

inline bool FieldMeta::isFixArray() const {
  return count_ > 1;
}

inline bool FieldMeta::isVarArray() const {
  return count_ == 0;
}

template <>
struct FieldMeta::GetDefaultImpl<bool> {
  static bool get(const dynamic& d) {
    return d.getBool();
  }
};
template <class T>
struct FieldMeta::GetDefaultImpl<
    T, typename std::enable_if<IsInt<T>::value>::type> {
  static T get(const dynamic& d) {
    return to<T>(d.getInt());
  }
};
template <class T>
struct FieldMeta::GetDefaultImpl<
    T, typename std::enable_if<IsFloat<T>::value>::type> {
  static T get(const dynamic& d) {
    return to<T>(d.getDouble());
  }
};
template <class T>
struct FieldMeta::GetDefaultImpl<
    T, typename std::enable_if<IsString<T>::value>::type> {
  static T get(const dynamic& d) {
    return d.getString();
  }
};

template <class T>
T FieldMeta::dflt() const {
  if (!checkType<T>(type_)) {
    CRYSTAL_THROW(RuntimeError,
                  "unmatch type: ", dataTypeToString(type_),
                  "!=", dataTypeToString(getType<T>()));
  }
  return GetDefaultImpl<T>::get(default_);
}

}  // namespace crystal
