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

#include "crystal/serializer/DynamicEncoding.h"

using namespace crystal;

TEST(DynamicEncoding, value) {
  int i = 0;
  decode(encode(100), i);
  EXPECT_EQ(100, i);

  float f = 0;
  decode(encode(123.456), f);
  EXPECT_FLOAT_EQ(123.456, f);

  std::string s;
  decode(encode(std::string("string")), s);
  EXPECT_STREQ("string", s.c_str());

  std::string p;
  decode(encode("string"), p);
  EXPECT_STREQ("string", p.c_str());
}

TEST(DynamicEncoding, pair) {
  std::pair<int, std::string> p;
  decode(encode(std::make_pair(100, "string")), p);
  EXPECT_EQ(100, p.first);
  EXPECT_STREQ("string", p.second.c_str());
}

TEST(DynamicEncoding, varadic) {
  int i;
  float f;
  std::string s;
  std::vector<int> v;
  vdecode(
      vencode(100, 123.456, "string", std::vector<int>({1,2,3})), i, f, s, v);
  EXPECT_EQ(100, i);
  EXPECT_FLOAT_EQ(123.456, f);
  EXPECT_STREQ("string", s.c_str());
  EXPECT_EQ(3, v.size());
  EXPECT_EQ(1, v[0]);
  EXPECT_EQ(2, v[1]);
  EXPECT_EQ(3, v[2]);
}
