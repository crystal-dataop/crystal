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

#include "crystal/dataframe/DocumentArray.h"
#include "crystal/strategy/Hash.h"
#include "crystal/type/Utility.h"

namespace crystal {
namespace op {

#define CRYSTAL_MERGE_TYPE_GEN(x) \
  x(Append),                      \
  x(And),                         \
  x(Or)

#define CRYSTAL_MERGE_TYPE_ENUM(type) k##type

enum class MergeType {
  CRYSTAL_MERGE_TYPE_GEN(CRYSTAL_MERGE_TYPE_ENUM)
};

#undef CRYSTAL_MERGE_TYPE_ENUM

const char* mergeTypeToString(MergeType type);

MergeType stringToMergeType(const char* str);

//////////////////////////////////////////////////////////////////////

template <class T>
inline void searchKV(DocumentArray& kv, const T& token) {
  CRYSTAL_LOG(DEBUG) << "search kv with token: " << token;
  uint64_t id = kv.object()->table()->find(hashToken(token));
  if (id == uint64_t(-1)) {
    CRYSTAL_LOG(DEBUG) << "token '" << token << "' not found";
  }
  uint16_t offset = kv.getTokenCount();
  auto& doc = kv.docs().emplaceTemp(kv.object(), offset, id);
#if CRYSTAL_CHECK_DELETE
  if (doc.isValid()) {
#endif
    kv.docs().increment();
#if CRYSTAL_CHECK_DELETE
  }
#endif
  kv.incrementTokenCount();
}

template <class T, class Container>
inline typename std::enable_if<
  IsContainer<Container>::value && std::is_arithmetic<T>::value>::type
msearchKV(DocumentArray& kv, const Container& tokens) {
  for (const auto& token : tokens) {
    searchKV(kv, to<T>(token));
  }
}

template <class T, class Container>
inline typename std::enable_if<
  IsContainer<Container>::value && IsString<T>::value &&
  IsString<typename ContainerValueType<Container>::type>::value>::type
msearchKV(DocumentArray& kv, const Container& tokens) {
  for (const auto& token : tokens) {
    searchKV(kv, token);
  }
}

template <class T, class Container>
inline typename std::enable_if<
  IsContainer<Container>::value && IsString<T>::value &&
  !IsString<typename ContainerValueType<Container>::type>::value>::type
msearchKV(DocumentArray&, const Container&) {
  CRYSTAL_LOG(ERROR) << "need string-like type";
}

template <class T>
inline void
msearchKV(DocumentArray& kv, std::string_view tokens) {
  auto convTokens = toVector<T>(tokens);
  for (const auto& token : convTokens) {
    searchKV(kv, token);
  }
}

//////////////////////////////////////////////////////////////////////

namespace detail {

void searchIndex(DocumentArray& index,
                 const std::string& indexName,
                 size_t indexNo,
                 uint64_t token,
                 uint16_t offset,
                 size_t limit,
                 MergeType mergeType);

} // namespace detail

template <class T>
inline void searchIndex(DocumentArray& index,
                        const std::string& indexName,
                        size_t indexNo,
                        const T& token,
                        size_t limit = -1,
                        MergeType mergeType = MergeType::kAppend) {
  CRYSTAL_LOG(DEBUG) << "search index with token: " << token
      << ", merge type: " << mergeTypeToString(mergeType);
  uint16_t offset = index.getTokenCount();
  detail::searchIndex(index,
                      indexName,
                      indexNo,
                      hashToken(token),
                      offset,
                      limit,
                      mergeType);
  if (mergeType != MergeType::kAnd) {
    index.incrementTokenCount();
  }
}

template <class T, class Container>
inline typename std::enable_if<
  IsContainer<Container>::value && std::is_arithmetic<T>::value>::type
msearchIndex(DocumentArray& index,
             const std::string& indexName,
             size_t indexNo,
             const Container& tokens,
             size_t limit,
             MergeType mergeType) {
  for (const auto& token : tokens) {
    searchIndex(index, indexName, indexNo, to<T>(token), limit, mergeType);
  }
}

template <class T, class Container>
inline typename std::enable_if<
  IsContainer<Container>::value && IsString<T>::value &&
  IsString<typename ContainerValueType<Container>::type>::value>::type
msearchIndex(DocumentArray& index,
             const std::string& indexName,
             size_t indexNo,
             const Container& tokens,
             size_t limit,
             MergeType mergeType) {
  for (const auto& token : tokens) {
    searchIndex(index, indexName, indexNo, token, limit, mergeType);
  }
}

template <class T, class Container>
inline typename std::enable_if<
  IsContainer<Container>::value && IsString<T>::value &&
  !IsString<typename ContainerValueType<Container>::type>::value>::type
msearchIndex(DocumentArray&,
             const std::string&,
             size_t,
             const Container&,
             size_t,
             MergeType) {
  CRYSTAL_LOG(ERROR) << "need string-like type";
}

template <class T>
inline void
msearchIndex(DocumentArray& index,
             const std::string& indexName,
             size_t indexNo,
             std::string_view tokens,
             size_t limit,
             MergeType mergeType) {
  auto convTokens = toVector<T>(tokens);
  for (const auto& token : convTokens) {
    searchIndex(index, indexName, indexNo, token, limit, mergeType);
  }
}

} // namespace op
} // namespace crystal
