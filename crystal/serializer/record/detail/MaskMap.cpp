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

#include "crystal/serializer/record/detail/MaskMap.h"

namespace crystal {

MaskMap::MaskMap() {
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      array_[i][j] = 0xfffffffffffffffful
                  << i >> i
                  >> (P - j) << (P - j);
    }
  }
}

} // namespace crystal
