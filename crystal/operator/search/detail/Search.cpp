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

#include "crystal/operator/search/detail/Search.h"

namespace crystal {
namespace op {

#define CRYSTAL_MERGE_TYPE_STR(method) #method

namespace {

static const char* sMergeTypeStrings[] = {
  CRYSTAL_MERGE_TYPE_GEN(CRYSTAL_MERGE_TYPE_STR)
};

}

const char* mergeTypeToString(MergeType type) {
  return sMergeTypeStrings[static_cast<unsigned>(type)];
}

MergeType stringToMergeType(const char* str) {
  size_t n = std::size(sMergeTypeStrings);
  for (size_t i = 0; i < n; ++i) {
    if (strcasecmp(str, sMergeTypeStrings[i]) == 0) {
      return static_cast<MergeType>(i);
    }
  }
  return MergeType::kAppend;
}

namespace detail {

void searchIndex(DocumentArray& index,
                 const std::string& indexName,
                 size_t indexNo,
                 uint64_t token,
                 uint16_t offset,
                 size_t limit,
                 MergeType mergeType) {
  auto postingList = index.object()->table()->getPostingList(indexName, token);
  if (isBlank(postingList)) {
    CRYSTAL_LOG(DEBUG) << "token '" << token << "' not found";
    return;
  }

  size_t n = get(postingList)->size();
  AnyPostingListIterator it = get(postingList)->iterator();
  if (n > limit) {
    n = limit;
  }
  CRYSTAL_LOG(DEBUG) << "token '" << token << "' got " << n << " docs";

  switch (mergeType) {
    case MergeType::kAnd: {
      if (!index.docs().empty()) {
        DocStorageArray mergedDocs;
        size_t k = 0;
        uint64_t id = index.docs()[k].id();

        for (size_t i = 0; i < n; ++i) {
          Document doc(index.object(),
                       offset,
                       get(it)->value()->data(),
                       get(postingList)->index(),
                       indexNo);
          get(it)->next();
#if CRYSTAL_CHECK_DELETE
          if (doc.isValid()) {
#endif
            while (id < doc.id()) {
              if (++k == index.docs().size()) {
                goto outOfAndLoop;
              }
              id = index.docs()[k].id();
            }
            if (id == doc.id()) {
              mergedDocs.emplace(std::move(index.docs()[k]));
              if (++k == index.docs().size()) {
                goto outOfAndLoop;
              }
              id = index.docs()[k].id();
            }
#if CRYSTAL_CHECK_DELETE
          }
#endif
        }
outOfAndLoop:
        index.docs().swap(mergedDocs);
      }
      // else: for empty, just break
      break;
    }
    case MergeType::kOr: {
      if (!index.docs().empty()) {
        DocStorageArray mergedDocs;
        size_t k = 0;
        uint64_t id = index.docs()[k].id();

        size_t i = 0;
        for (; i < n; ++i) {
          Document doc(index.object(),
                       offset,
                       get(it)->value()->data(),
                       get(postingList)->index(),
                       indexNo);
          get(it)->next();
#if CRYSTAL_CHECK_DELETE
          if (doc.isValid()) {
#endif
            while (id < doc.id()) {
              mergedDocs.emplace(std::move(index.docs()[k]));
              if (++k == index.docs().size()) {
                break;
              }
              id = index.docs()[k].id();
            }
            if (id == doc.id()) {
              mergedDocs.emplace(std::move(index.docs()[k]));
              if (++k != index.docs().size()) {
                id = index.docs()[k].id();
              }
            } else {
              mergedDocs.emplace(std::move(doc));
            }
            if (k == index.docs().size()) {
              break;
            }
#if CRYSTAL_CHECK_DELETE
          }
#endif
        }
        for (++i; i < n; ++i) {
          auto& doc = mergedDocs.emplaceTemp(index.object(),
                                             offset,
                                             get(it)->value()->data(),
                                             get(postingList)->index(),
                                             indexNo);
          get(it)->next();
#if CRYSTAL_CHECK_DELETE
          if (doc.isValid()) {
#endif
            mergedDocs.increment();
#if CRYSTAL_CHECK_DELETE
          }
#endif
        }
        for (; k < index.docs().size(); ++k) {
          mergedDocs.emplace(std::move(index.docs()[k]));
        }
        index.docs().swap(mergedDocs);
        break;
      }
      // else: for empty, fall through to append mode
    }
    case MergeType::kAppend: {
      for (size_t i = 0; i < n; ++i) {
        auto& doc = index.docs().emplaceTemp(index.object(),
                                             offset,
                                             get(it)->value()->data(),
                                             get(postingList)->index(),
                                             indexNo);
        get(it)->next();
#if CRYSTAL_CHECK_DELETE
        if (doc.isValid()) {
#endif
          index.docs().increment();
#if CRYSTAL_CHECK_DELETE
        }
#endif
      }
      break;
    }
  }
}

} // namespace detail

} // namespace op
} // namespace crystal
