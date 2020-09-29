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

#include "crystal/storage/index/bitmap/BitmapPosting.h"

#include "crystal/foundation/Logging.h"

namespace crystal {

bool BitmapPosting::parseFrom(const Record& record) {
  const FieldMeta* meta = record.recordMeta()->getMeta("__id");
  if (meta == nullptr) {
    CRYSTAL_LOG(ERROR) << "__id field not found";
    return false;
  }
  id = record.get<uint64_t>(*meta);
  return true;
}

bool BitmapPosting::update(const Record&) {
  CRYSTAL_LOG(ERROR) << "update not support";
  return false;
}

}  // namespace crystal
