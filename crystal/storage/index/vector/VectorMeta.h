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

#include <cstdint>
#include <string>

namespace crystal {

#define CRYSTAL_VECTOR_TYPE_GEN(x)  \
  x(Faiss)

#define CRYSTAL_VECTOR_TYPE_ENUM(type) type

enum class VectorType {
  CRYSTAL_VECTOR_TYPE_GEN(CRYSTAL_VECTOR_TYPE_ENUM)
};

#undef CRYSTAL_VECTOR_TYPE_ENUM

const char* vectorTypeToString(VectorType type);

int stringToFaissMetric(const char* str);

struct VectorMeta {
  VectorType type;
  std::string desc;
  int64_t dimension;
  int metric;
  int64_t trainSize{1 << 20};
  int64_t batchSize{1 << 16};
};

}  // namespace crystal
