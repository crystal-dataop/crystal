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

#include "crystal/serializer/record/detail/Bitset.h"

using namespace crystal;

TEST(Bitset, all) {
  Bitset bitset;

  bitset.assign(0x1);

  EXPECT_EQ(0x1, bitset.retrieve());
  EXPECT_EQ(64, bitset.count());

  EXPECT_TRUE(bitset.test(0));

  EXPECT_TRUE(bitset.unset(0));
  EXPECT_EQ(0x0, bitset.retrieve());

  EXPECT_TRUE(bitset.set(0));
  EXPECT_EQ(0x1, bitset.retrieve());

  bitset = Bitset(4);

  EXPECT_EQ(0x0, bitset.retrieve());
  EXPECT_EQ(4, bitset.count());

  bitset = Bitset(4, 0xf);

  EXPECT_EQ(0xf, bitset.retrieve());
  EXPECT_EQ(4, bitset.count());

  uint64_t buf[2];
  uint8_t* p = reinterpret_cast<uint8_t*>(buf);

  for (size_t i = 0; i < 64; ++i) {
    Bitset b(4);
    EXPECT_TRUE(bitset.serialize(p, i));
    EXPECT_TRUE(b.deserialize(p, i));
    EXPECT_EQ(0xf, b.retrieve());
  }
}
