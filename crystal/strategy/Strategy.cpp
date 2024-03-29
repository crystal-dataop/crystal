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

#include "crystal/strategy/Strategy.h"

#include "crystal/strategy/DefaultStrategy.h"

namespace crystal {

std::unique_ptr<Strategy>
createStrategy(StrategyType type, const FieldMeta& keyMeta) {
  switch (type) {
    case StrategyType::kDefault:
      return std::unique_ptr<Strategy>(new DefaultStrategy(keyMeta));
  }
  return nullptr;
}

} // namespace crystal
