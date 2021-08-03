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

#include "crystal/serializer/record/AllocMask.h"

using namespace crystal;

TEST(mask, get_and_set) {
  uint8_t h = 0;
  EXPECT_FALSE(getMask(&h));
  EXPECT_FALSE(h & 0x1);
  setMask(&h);
  EXPECT_TRUE(getMask(&h));
  EXPECT_TRUE(h & 0x1);
}

TEST(bytes, get) {
  uint8_t h = 0;
  EXPECT_EQ(1, getBytes(&h));
  EXPECT_EQ(0, h & 0xe);
  reinterpret_cast<Head*>(&h)->nvar = 1;
  EXPECT_EQ(2, getBytes(&h));
  EXPECT_EQ(0x2, h & 0xe);
  reinterpret_cast<Head*>(&h)->nvar = 3;
  EXPECT_EQ(4, getBytes(&h));
  EXPECT_EQ(0x6, h & 0xe);
  reinterpret_cast<Head*>(&h)->nvar = 7;
  EXPECT_EQ(8, getBytes(&h));
  EXPECT_EQ(0xe, h & 0xe);
}

TEST(size, get_and_set) {
  uint8_t bytes[8];
  EXPECT_EQ(0, getSize(bytes));
  setSize(bytes, 0xf);
  EXPECT_EQ(0xf, getSize(bytes));
  setSize(bytes, 0xfff);
  EXPECT_EQ(0xfff, getSize(bytes));
  setSize(bytes, 0xfffffff);
  EXPECT_EQ(0xfffffff, getSize(bytes));
  setSize(bytes, 0xfffffffffffffff);
  EXPECT_EQ(0xfffffffffffffff, getSize(bytes));
}
