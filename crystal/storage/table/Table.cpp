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

#include "crystal/storage/table/Table.h"

#include <filesystem>
#include <string>
#include <string_view>

#include "crystal/foundation/Conv.h"
#include "crystal/foundation/Logging.h"
#include "crystal/foundation/String.h"

namespace fs = std::filesystem;

namespace crystal {

Table::Table(const TableConfig& config)
    : config_(config),
      recordMeta_(buildRecordMeta(config.recordConfig())) {
}

bool Table::init(const char* path, bool readOnly) {
  for (auto& kv : config_.kvConfig().fields()) {
    kvFields_.insert(kv.first);
  }
  for (auto& kv : config_.indexConfigs()) {
    indexes_.push_back(kv.first);
    for (auto& p : kv.second.fields()) {
      auto& indexSet = indexFields_[p.first];
      indexSet.first = true;
      indexSet.second.set(indexes_.size() - 1);
    }
  }
  path_ = path;
  readOnly_ = readOnly;
  if (!readOnly_) {
    fs::create_directories(path_);
  }
  return initKV() && initIndex();
}

bool Table::initKV() {
  auto kvDir = fs::path(path_) / "kv";
  auto& kvConfig = config_.kvConfig();
  if (kvConfig.fields().empty()) {
    CRYSTAL_LOG(INFO) << "empty kv in table: " <<  config_.name();
    return true;
  }
  fs::create_directory(kvDir);
  if (fs::is_empty(kvDir)) {
    if (readOnly_) {
      CRYSTAL_LOG(ERROR) << "list directory '" << kvDir << "' failed";
      return false;
    }
    for (uint16_t i = 0; i < kvConfig.segment(); ++i) {
      fs::create_directory(toString(kvDir / "segment", i));
    }
  }
  std::vector<fs::path> kvSegmentDirs;
  for (auto& dir : fs::directory_iterator(kvDir)) {
    kvSegmentDirs.push_back(dir);
  }
  if (kvSegmentDirs.size() != kvConfig.segment()) {
    CRYSTAL_LOG(WARN) << "unmatch kv segment count with conf for: " << kvDir;
  }
  kvs_.resize(kvSegmentDirs.size());
  for (auto& dir : kvSegmentDirs) {
    std::string dirName = dir.filename();
    std::string_view segmentDir = dirName;
    if (!removePrefix(segmentDir, "segment")) {
      CRYSTAL_LOG(ERROR) << "invalid segment prefix, directory: " << dir;
      return false;
    }
    size_t segmentNo = to<size_t>(segmentDir);
    if (segmentNo >= kvSegmentDirs.size()) {
      CRYSTAL_LOG(ERROR) << "invalid segment no: " << segmentNo;
      return false;
    }
    auto file = dir / "mmap";
    MemoryManager* memory = new MemoryManager(file.c_str(), readOnly_);
    KV* kv = new KV(kvConfig);
    if (!kv->init(memory)) {
      CRYSTAL_LOG(ERROR) << "init kv failed";
      return false;
    }
    memorys_.emplace_back(memory);
    kvs_[segmentNo].reset(kv);
  }
  CRYSTAL_LOG(INFO) << "init kv at table directory: " << path_;
  return true;
}

bool Table::initIndex() {
  auto indexRoot = fs::path(path_) / "index";
  auto& indexConfigs = config_.indexConfigs();
  if (indexConfigs.empty()) {
    CRYSTAL_LOG(INFO) << "empty index in table: " << config_.name();
    return true;
  }
  fs::create_directory(indexRoot);
  if (fs::is_empty(indexRoot)) {
    if (readOnly_) {
      CRYSTAL_LOG(ERROR) << "list directory '" << indexRoot << "' failed";
      return false;
    }
    for (auto& kv : indexConfigs) {
      fs::create_directory(indexRoot / kv.first);
    }
  }
  std::vector<std::pair<std::string, fs::path>> indexFieldAndDirs;
  for (auto& dir : fs::directory_iterator(indexRoot)) {
    indexFieldAndDirs.push_back(std::make_pair(dir.path().filename(), dir));
  }
  if (indexFieldAndDirs.size() != indexConfigs.size()) {
    CRYSTAL_LOG(ERROR) << "index count in directory '" << path_
        << "' != index count in config";
    return false;
  }
  for (auto& p : indexFieldAndDirs) {
    auto& index = p.first;
    auto& indexDir = p.second;
    auto it = indexConfigs.find(index);
    if (it == indexConfigs.end()) {
      CRYSTAL_LOG(ERROR) << "index '" << index << "' not found";
      return false;
    }
    const IndexConfig& indexConfig = it->second;
    if (fs::is_empty(indexDir)) {
      if (readOnly_) {
        CRYSTAL_LOG(ERROR) << "list directory '" << indexDir << "' failed";
        return false;
      }
      for (uint16_t i = 0; i < indexConfig.segment(); ++i) {
        fs::create_directory(toString(indexDir / "segment", i));
      }
    }
    std::vector<fs::path> indexSegmentDirs;
    for (auto& dir : fs::directory_iterator(indexDir)) {
      indexSegmentDirs.push_back(dir);
    }
    if (indexSegmentDirs.size() != indexConfig.segment()) {
      CRYSTAL_LOG(WARN)
          << "unmatch index segment count with conf for: " << indexDir;
    }
    std::vector<std::unique_ptr<Index>> indexes;
    indexes.resize(indexSegmentDirs.size());
    for (auto& dir : indexSegmentDirs) {
      std::string dirName = dir.filename();
      std::string_view segmentDir = dirName;
      if (!removePrefix(segmentDir, "segment")) {
        CRYSTAL_LOG(ERROR) << "invalid segment prefix, directory: " << dir;
        return false;
      }
      size_t segmentNo = to<size_t>(segmentDir);
      if (segmentNo >= indexSegmentDirs.size()) {
        CRYSTAL_LOG(ERROR) << "invalid segment no: " << segmentNo;
        return false;
      }
      auto file = dir / "mmap";
      MemoryManager* memory = new MemoryManager(file.c_str(), readOnly_);
      Index* index = new Index(indexConfig);
      if (!index->init(memory)) {
        CRYSTAL_LOG(ERROR) << "init index failed";
        return false;
      }
      memorys_.emplace_back(memory);
      indexes[segmentNo].reset(index);
    }
    indexMap_[indexConfig.key()] = std::move(indexes);
  }
  CRYSTAL_LOG(INFO) << "init index at table directory: " << path_;
  return true;
}

void Table::dump() {
  for (auto& memory : memorys_) {
    memory->dump();
  }
}

size_t Table::getNoOfIndex(const std::string& index) const {
  for (size_t i = 0; i < indexes_.size(); ++i) {
    if (index == indexes_[i]) {
      return i;
    }
  }
  return -1;
}

uint64_t Table::find(uint64_t key) const {
  if (kvs_.empty()) {
    return -1;
  }
  uint64_t seg = key % kvs_.size();
  if (seg > uint16_t(-1)) {
    return -1;
  }
  uint32_t segOffset = kvs_[seg]->find(key);
  if (segOffset == uint32_t(-1)) {
    return -1;
  }
  return (seg << 32) | segOffset;
}

bool Table::insert(uint64_t key, uint64_t id) {
  if (kvs_.empty()) {
    return false;
  }
  uint64_t seg = key % kvs_.size();
  if (seg > uint16_t(-1)) {
    return false;
  }
  return kvs_[seg]->insert(key, uint32_t(id));
}

bool Table::erase(uint64_t key) {
  if (kvs_.empty()) {
    return false;
  }
  uint64_t seg = key % kvs_.size();
  if (seg > uint16_t(-1)) {
    return false;
  }
  kvs_[seg]->erase(key);
  return true;
}

const std::vector<std::unique_ptr<Index>>*
Table::getIndexSegments(const std::string& index) const {
  auto it = indexMap_.find(index);
  return it != indexMap_.end() ? &it->second : nullptr;
}

size_t Table::getIndexSegmentCount(const std::string& index) const {
  auto it = indexMap_.find(index);
  return it != indexMap_.end() ? it->second.size() : 0;
}

Index* Table::getIndexSegment(const std::string& index, uint16_t seg) const {
  auto it = indexMap_.find(index);
  return it != indexMap_.end()
      ? (seg < it->second.size()
         ? it->second[seg].get() : nullptr) : nullptr;
}

Index* Table::getIndexSegmentByToken(
    const std::string& index, uint64_t token) const {
  auto it = indexMap_.find(index);
  return it != indexMap_.end()
      ? it->second[token % it->second.size()].get()
      : nullptr;
}

AnyPostingList Table::getPostingList(
    const std::string& index, uint64_t token) const {
  Index* idx = getIndexSegmentByToken(index, token);
  return idx ? idx->getPostingList(token) : std::monostate();
}

}  // namespace crystal
