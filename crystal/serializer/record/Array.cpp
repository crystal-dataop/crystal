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

#include "crystal/serializer/record/Array.h"

namespace crystal {

Array<std::string> Array<std::string_view>::toStdStringArray() const {
  return Array<std::string>(ptr_, size_, alloc_, default_, bitMask_);
}

bool Array<std::string_view>::set(size_t i, const std::string_view& value) {
  bitMask_.set(true);
  int64_t& offset = ptr_[i];
  if (offset != 0) {
    alloc_->deallocate(offset);
    offset = 0;
  }
  int64_t newOffset = writeBuffer(alloc_, value.data(), value.size());
  if (newOffset == 0) {
    return false;
  }
  offset = newOffset;
  return true;
}

bool Array<std::string>::set(size_t i, const std::string& value) {
  bitMask_.set(true);
  int64_t& offset = ptr_[i];
  if (offset != 0) {
    alloc_->deallocate(offset);
    offset = 0;
  }
  int64_t newOffset = writeBuffer(alloc_, value.data(), value.size());
  if (newOffset == 0) {
    return false;
  }
  offset = newOffset;
  return true;
}

}  // namespace crystal
