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

#include "crystal/dataframe/NumericIndexArray.h"

using namespace crystal;

TEST(U32IndexArray, all) {
  U32IndexArray array;

  array.resize(1000);
  EXPECT_EQ(1000, array.size());
  EXPECT_EQ(1024, array.capacity());

  uint32_t i = 0;
  for (uint32_t v : array) {
    EXPECT_EQ(i++, v);
  }

  array.resize(11000);
  EXPECT_EQ(11000, array.size());
  EXPECT_EQ(16384, array.capacity());

  i = 0;
  for (uint32_t v : array) {
    EXPECT_EQ(i++, v);
  }

  array.resize(12000);
  EXPECT_EQ(12000, array.size());
  EXPECT_EQ(16384, array.capacity());

  i = 0;
  for (uint32_t v : array) {
    EXPECT_EQ(i++, v);
  }

  array.push_back(100);
  EXPECT_EQ(12001, array.size());
  array.pop_back();
  EXPECT_EQ(12000, array.size());
}
