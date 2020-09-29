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

#include "crystal/strategy/StrategyType.h"

#include <iterator>
#include <strings.h>

namespace crystal {

#define CRYSTAL_STRATEGY_TYPE_STR(type) #type

static const char* sStrategyTypeStrings[] = {
  CRYSTAL_STRATEGY_TYPE_GEN(CRYSTAL_STRATEGY_TYPE_STR)
};

#undef CRYSTAL_STRATEGY_TYPE_STR

const char* strategyTypeToString(StrategyType type) {
  return sStrategyTypeStrings[static_cast<int>(type)];
}

StrategyType stringToStrategyType(const char* str) {
  size_t n = std::size(sStrategyTypeStrings);
  for (size_t i = 0; i < n; ++i) {
    if (strcasecmp(str, sStrategyTypeStrings[i]) == 0) {
      return static_cast<StrategyType>(i);
    }
  }
  return StrategyType::kDefault;
}

} // namespace crystal
