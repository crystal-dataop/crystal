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

#include <gtest/gtest.h>

#include "crystal/serializer/record/detail/MaskMap.h"

using namespace crystal;

TEST(MaskMap, all) {
  MaskMap map;

  EXPECT_EQ(0x1, map.test(0xf, 0));
  EXPECT_EQ(0x2, map.test(0xf, 1));
  EXPECT_EQ(0x4, map.test(0xf, 2));
  EXPECT_EQ(0x8, map.test(0xf, 3));

  EXPECT_EQ(0x1, map.set(0x0, 0));
  EXPECT_EQ(0x2, map.set(0x0, 1));
  EXPECT_EQ(0x4, map.set(0x0, 2));
  EXPECT_EQ(0x8, map.set(0x0, 3));

  EXPECT_EQ(0xe, map.unset(0xf, 0));
  EXPECT_EQ(0xd, map.unset(0xf, 1));
  EXPECT_EQ(0xb, map.unset(0xf, 2));
  EXPECT_EQ(0x7, map.unset(0xf, 3));

  EXPECT_EQ(0x0, map.lowBitset(0xf, 0));
  EXPECT_EQ(0x1, map.lowBitset(0xf, 1));
  EXPECT_EQ(0x3, map.lowBitset(0xf, 2));
  EXPECT_EQ(0x7, map.lowBitset(0xf, 3));

  EXPECT_EQ(0xf000000000000000, map.mask(0, 4));
  EXPECT_EQ(0x7800000000000000, map.mask(1, 4));
  EXPECT_EQ(0x3c00000000000000, map.mask(2, 4));
  EXPECT_EQ(0x1e00000000000000, map.mask(3, 4));
}
