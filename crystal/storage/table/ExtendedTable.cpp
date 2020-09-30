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

#include "crystal/storage/table/ExtendedTable.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

ExtendedTable::ExtendedTable(
    Table* table, TableGroup* tableGroup, bool extend)
    : table_(table), tableGroup_(tableGroup) {
  auto& config = table_->config();

  for (auto& p : config.recordConfig()) {
    auto meta = table_->recordMeta().getMeta(p.first);
    const IndexNoSet* indexNoSet = nullptr;
    if (table_->fieldInIndex(p.first, indexNoSet)) {
      int type = table_->fieldInKV(p.first)
               ? FieldInfo::kPayloadAndValue
               : FieldInfo::kPayload;
      buildFieldInfo(*meta, type, indexNoSet);
    } else if (table_->fieldInKV(p.first)) {
      buildFieldInfo(*meta, FieldInfo::kValue);
    } else {
      buildFieldInfo(*meta, FieldInfo::kUnUsed);
    }
  }
  fieldIndex_.build(fields_);

  if (extend) {
    int no = 0;
    for (auto& kv : config.relatedTables()) {
      auto* relatedTable = tableGroup_->getTable(kv.second);
      if (relatedTable) {
        auto& relatedConfig = relatedTable->config();
        for (auto& p : relatedConfig.kvConfig().fields()) {
          FieldInfo::Related related;
          related.no = no;
          related.ref = fieldIndex_.getIndexOfField(kv.first);
          related.table = relatedTable;
          buildFieldInfo(*relatedTable->recordMeta().getMeta(p.first),
                         FieldInfo::kRelated,
                         nullptr,
                         related);
        }
        ++no;
      }
    }
  }
  fieldIndex_.build(fields_);
}

void ExtendedTable::buildFieldInfo(
    const FieldMeta& meta,
    int type,
    const IndexNoSet* set,
    FieldInfo::Related related) {
  std::string name = (related.table
      ? related.table->config().name() + "__"
      : "") + meta.name();
  CRYSTAL_LOG(DEBUG) << "build info for field: " << name
      << ", type=" << fieldInfoTypeToString(type);
  FieldInfo fi;
  fi.meta = meta;
  fi.type = type;
  if (set) {
    fi.indexNo = *set;
  }
  fi.related = related;
  fields_.emplace_back(name);
  fieldInfos_.push_back(fi);
}

bool ExtendedTable::hasKV() const {
  return table_->getKVSegmentCount() != 0;
}

DataType ExtendedTable::getFieldType(const std::string& field) const {
  for (auto& fi : fieldInfos_) {
    if (field == fi.meta.name()) {
      return fi.meta.type();
    }
  }
  return DataType::UNKNOWN;
}

} // namespace crystal
