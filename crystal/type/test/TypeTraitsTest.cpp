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

#include <gtest/gtest.h>

#include "crystal/type/TypeTraits.h"

using namespace crystal;

TEST(TypeTraits, all) {
  EXPECT_TRUE(IsValue<bool>::value);
  EXPECT_TRUE(IsValue<int8_t>::value);
  EXPECT_TRUE(IsValue<float>::value);
  EXPECT_TRUE(IsValue<double>::value);
  EXPECT_TRUE(IsValue<std::string_view>::value);
}
