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

namespace crystal {

#define CRYSTAL_INDEX_TYPE_GEN(x) \
  x(None),                        \
  x(Bitmap)

#define CRYSTAL_INDEX_TYPE_ENUM(type) k##type

enum class IndexType {
  CRYSTAL_INDEX_TYPE_GEN(CRYSTAL_INDEX_TYPE_ENUM)
};

#undef CRYSTAL_INDEX_TYPE_ENUM

const char* indexTypeToString(IndexType type);

IndexType stringToIndexType(const char* str);

}  // namespace crystal
