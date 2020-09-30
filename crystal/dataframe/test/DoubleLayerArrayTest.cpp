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

#include "crystal/dataframe/DoubleLayerArray.h"

using namespace crystal;

TEST(DoubleLayerArray, all) {
  DoubleLayerArray<uint64_t> array(1024);

  uint64_t i;
  for (i = 0; i < 1000; ++i) {
    EXPECT_EQ(i, array.emplace(i));
  }
  EXPECT_EQ(1000, array.size());
  EXPECT_EQ(1024, array.capacity());

  i = 0;
  for (auto& v : array) {
    EXPECT_EQ(i++, v);
  }

  array.resize(200);
  EXPECT_EQ(200, array.size());
  EXPECT_EQ(1024, array.capacity());
  array.resize(1000);
  EXPECT_EQ(1000, array.size());
  EXPECT_EQ(1024, array.capacity());

  for (i = 200; i < 1000; ++i) {
    EXPECT_EQ(0, array[i]);
  }
}
