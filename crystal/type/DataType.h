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

#include <cstddef>
#include <string_view>
#include <type_traits>

#include "crystal/foundation/Exception.h"
#include "crystal/math/Div.h"

namespace crystal {

#define CRYSTAL_DATA_TYPE_GEN(x)  \
  x(UNKNOWN),                     \
  x(BOOL),                        \
  x(INT8),                        \
  x(INT16),                       \
  x(INT32),                       \
  x(INT64),                       \
  x(UINT8),                       \
  x(UINT16),                      \
  x(UINT32),                      \
  x(UINT64),                      \
  x(FLOAT),                       \
  x(DOUBLE),                      \
  x(STRING)

#define CRYSTAL_DATA_TYPE_ENUM(type) type

enum class DataType {
  CRYSTAL_DATA_TYPE_GEN(CRYSTAL_DATA_TYPE_ENUM)
};

#undef CRYSTAL_DATA_TYPE_ENUM

const char* dataTypeToString(DataType type);

DataType stringToDataType(const char* str);

bool isBool(DataType type);
bool isIntegral(DataType type);
bool isFloat(DataType type);
bool isArithmetic(DataType type);
bool isString(DataType type);

size_t sizeOf(DataType type);

template <class T>
struct DataTypeTraits {
  enum {
    value = static_cast<int>(DataType::UNKNOWN)
  };
};

#define CRYSTAL_DATA_TYPE_TRAITS(type, enum_type)   \
  template <>                                       \
  struct DataTypeTraits<type> {                     \
    enum {                                          \
      value = static_cast<int>(DataType::enum_type) \
    };                                              \
  };

CRYSTAL_DATA_TYPE_TRAITS(bool, BOOL)
CRYSTAL_DATA_TYPE_TRAITS(int8_t, INT8)
CRYSTAL_DATA_TYPE_TRAITS(int16_t, INT16)
CRYSTAL_DATA_TYPE_TRAITS(int32_t, INT32)
CRYSTAL_DATA_TYPE_TRAITS(int64_t, INT64)
CRYSTAL_DATA_TYPE_TRAITS(uint8_t, UINT8)
CRYSTAL_DATA_TYPE_TRAITS(uint16_t, UINT16)
CRYSTAL_DATA_TYPE_TRAITS(uint32_t, UINT32)
CRYSTAL_DATA_TYPE_TRAITS(uint64_t, UINT64)
CRYSTAL_DATA_TYPE_TRAITS(float, FLOAT)
CRYSTAL_DATA_TYPE_TRAITS(double, DOUBLE)
CRYSTAL_DATA_TYPE_TRAITS(std::string, STRING)
CRYSTAL_DATA_TYPE_TRAITS(std::string_view, STRING)

#undef CRYSTAL_DATA_TYPE_TRAITS

template <class T>
inline DataType getType() {
  return static_cast<DataType>(DataTypeTraits<T>::value);
}

template <class T>
inline bool checkType(DataType type) {
  return type == getType<T>();
}

//////////////////////////////////////////////////////////////////////

inline bool isBool(DataType type) {
  return type == DataType::BOOL;
}

inline bool isIntegral(DataType type) {
  switch (type) {
    case DataType::INT8:
    case DataType::INT16:
    case DataType::INT32:
    case DataType::INT64:
    case DataType::UINT8:
    case DataType::UINT16:
    case DataType::UINT32:
    case DataType::UINT64:
      return true;
    default:
      return false;
  }
}

inline bool isFloat(DataType type) {
  switch (type) {
    case DataType::FLOAT:
    case DataType::DOUBLE:
      return true;
    default:
      return false;
  }
}

inline bool isArithmetic(DataType type) {
  return isIntegral(type) || isFloat(type);
}

inline bool isString(DataType type) {
  return type == DataType::STRING;
}

inline size_t sizeOf(DataType type) {
  switch (type) {
    case DataType::BOOL: return div8(1);
    case DataType::INT8: return sizeof(int8_t);
    case DataType::INT16: return sizeof(int16_t);
    case DataType::INT32: return sizeof(int32_t);
    case DataType::INT64: return sizeof(int64_t);
    case DataType::UINT8: return sizeof(uint8_t);
    case DataType::UINT16: return sizeof(uint16_t);
    case DataType::UINT32: return sizeof(uint32_t);
    case DataType::UINT64: return sizeof(uint64_t);
    case DataType::FLOAT: return sizeof(float);
    case DataType::DOUBLE: return sizeof(double);
    case DataType::STRING: return sizeof(void*);
    default:
      CRYSTAL_THROW(
          RuntimeError, "unsupport data type: ", dataTypeToString(type));
  }
}

}  // namespace crystal
