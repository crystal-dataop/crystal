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

#include "crystal/storage/index/IndexBase.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

#define CRYSTAL_LOG_INDEX(level) \
  CRYSTAL_LOG(level) << config_->type() << " index '" << config_->key() << "' "

IndexBase::IndexBase(const IndexConfig* config)
    : config_(config),
      recordMeta_(buildRecordMeta(config->fields(), true)),
      keyMeta_(config->keyConfig().toFieldMeta()),
      accessor_(recordMeta_) {
}

bool IndexBase::add(uint64_t key, const Record& record) {
  AnyPostingList postingList = getPostingList(key);
  if (isBlank(postingList)) {
    createPostingList(key);
    postingList = getPostingList(key);
    if (isBlank(postingList)) {
      CRYSTAL_LOG_INDEX(ERROR) << "get posting list failed";
      return false;
    }
  }
  AnyPosting posting = get(postingList)->newPosting();
  if (isBlank(posting)) {
    CRYSTAL_LOG_INDEX(ERROR) << "new posting failed";
    return false;
  }
  if (!get(posting)->parseFrom(record)) {
    CRYSTAL_LOG_INDEX(ERROR) << "parse from record failed";
    return false;
  }
  int ret = get(postingList)->add(*get(posting));
  if (ret == -1) {
    CRYSTAL_LOG_INDEX(ERROR) << "add posting failed";
    get(posting)->reset();
    return false;
  }
  if (ret == 1) {
    AnyPosting onlinePosting =
        get(postingList)->getOnlinePosting(get(posting)->id);
    if (isBlank(onlinePosting)) {
      get(posting)->reset();
      CRYSTAL_LOG_INDEX(ERROR) << "add posting failed";
      return false;
    }
    bool r = get(onlinePosting)->update(record);
    get(posting)->reset();
    if (!r) {
      CRYSTAL_LOG_INDEX(ERROR) << "add posting failed";
      return false;
    }
  }
  return true;
}

bool IndexBase::update(uint64_t key, const Record& record) {
  AnyPostingList postingList = getPostingList(key);
  if (isBlank(postingList)) {
    CRYSTAL_LOG_INDEX(ERROR) << "get posting list failed";
    return false;
  }
  AnyPosting posting = get(postingList)->newPosting();
  if (isBlank(posting)) {
    CRYSTAL_LOG_INDEX(ERROR) << "new posting failed";
    return false;
  }
  if (!get(posting)->parseFrom(record)) {
    CRYSTAL_LOG_INDEX(ERROR) << "parse from record failed";
    return false;
  }
  AnyPosting onlinePosting =
      get(postingList)->getOnlinePosting(get(posting)->id);
  if (isBlank(onlinePosting)) {
    get(posting)->reset();
    CRYSTAL_LOG_INDEX(ERROR) << "update posting failed";
    return false;
  }
  bool ret = get(onlinePosting)->update(record);
  get(posting)->reset();
  if (!ret) {
    CRYSTAL_LOG_INDEX(ERROR) << "update posting failed";
    return false;
  }
  return true;
}

bool IndexBase::remove(uint64_t key, const Record& record) {
  AnyPostingList postingList = getPostingList(key);
  if (isBlank(postingList)) {
    CRYSTAL_LOG_INDEX(ERROR) << "get posting list failed";
    return false;
  }
  AnyPosting posting = get(postingList)->newPosting();
  if (isBlank(posting)) {
    CRYSTAL_LOG_INDEX(ERROR) << "new posting failed";
    return false;
  }
  if (!get(posting)->parseFrom(record)) {
    CRYSTAL_LOG_INDEX(ERROR) << "parse from record failed";
    return false;
  }
  if (!get(postingList)->remove(get(posting)->id)) {
    CRYSTAL_LOG_INDEX(ERROR) << "remove posting failed";
    return false;
  }
  return true;
}

bool IndexBase::bulkLoad(uint64_t key, const std::vector<Record>& records) {
  if (records.empty()) {
    return true;
  }
  AnyPostingList postingList = getPostingList(key);
  if (!isBlank(postingList) && get(postingList)->size() > 0) {
    CRYSTAL_LOG_INDEX(ERROR) << "posting list already exist";
    return false;
  }
  createPostingList(key);
  postingList = getPostingList(key);
  if (isBlank(postingList)) {
    CRYSTAL_LOG_INDEX(ERROR) << "get posting list failed";
    return false;
  }
  std::vector<AnyPosting> postings(records.size());
  if (!get(postingList)->newPostings(postings)) {
    CRYSTAL_LOG_INDEX(ERROR) << "new postings failed";
    return false;
  }
  for (size_t i = 0; i < postings.size(); ++i) {
    if (!get(postings[i])->parseFrom(records[i])) {
      CRYSTAL_LOG_INDEX(ERROR) << "parse from record failed";
      return false;
    }
  }
  if (get(postingList)->bulkLoad(postings) != 0) {
    CRYSTAL_LOG_INDEX(ERROR) << "bulkLoad postings failed";
    return false;
  }
  return true;
}

}  // namespace crystal
