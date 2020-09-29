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

#include "crystal/serializer/DynamicEncoding.h"

using namespace crystal;

TEST(DynamicEncoding, tuple) {
  std::tuple<int, float, std::string> t;
  decode(encode(std::make_tuple(100, 123.456, "string")), t);
  EXPECT_EQ(100, std::get<0>(t));
  EXPECT_FLOAT_EQ(123.456, std::get<1>(t));
  EXPECT_STREQ("string", std::get<2>(t).c_str());
}

TEST(DynamicEncoding, sequence) {
  std::vector<int> v;
  decode(encode(std::vector<int>({100, 200})), v);
  EXPECT_EQ(2, v.size());
  EXPECT_EQ(100, v[0]);
  EXPECT_EQ(200, v[1]);
}

TEST(DynamicEncoding, set) {
  std::set<int> s;
  decode(encode(std::set<int>({100, 200})), s);
  EXPECT_EQ(2, s.size());
  EXPECT_EQ(1, s.count(100));
  EXPECT_EQ(1, s.count(200));
}

TEST(DynamicEncoding, map) {
  std::map<int, std::string> m;
  decode(encode(std::map<int, std::string>({{100, "abc"}, {200, "xyz"}})), m);
  EXPECT_EQ(2, m.size());
  EXPECT_STREQ("abc", m[100].c_str());
  EXPECT_STREQ("xyz", m[200].c_str());
}
