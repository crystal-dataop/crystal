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

#include "crystal/foundation/String.h"
#include "crystal/foundation/TermColor.h"

namespace crystal {

std::string TermColorWrapper::operator()(const std::string& s, bool end) const {
  std::string out = "\x1b[";
  if (bold_) toAppend(&out, "1;");
  if (underline_) toAppend(&out, "4;");
  if (color_ >= 0) toAppend(&out, color_, ';');
  if (backgroundColor_ >= 0) toAppend(&out, backgroundColor_, ';');
  out.back() = 'm';
  toAppend(&out, s);
  if (end) {
    toAppend(&out, "\x1b[0m");
  }
  return out;
}

} // namespace crystal
