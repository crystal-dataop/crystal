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

#include "crystal/serializer/record/containers/Tuple.h"
#include "crystal/serializer/record/containers/TupleMetaGenerator.h"

using namespace crystal;

class TupleTest : public ::testing::Test {
 protected:
  untyped_tuple::meta meta =
    generateTupleMeta<
      tuple<
        int32_t,
        string,
        array<int, 10>,
        vector<string>,
        tuple<int, string>>>();
};

TEST_F(TupleTest, get_and_set) {
  tuple<
    int32_t,
    string,
    array<int, 10>,
    vector<string>,
    tuple<int, string>> t;
  t.untyped_tuple_.reset(meta);

  auto& r = t.untyped_tuple_;

  r.get<int32_t>(0) = 100;
  r.get<string>(1) = "string";
  auto& a = r.get<array<int, 10>>(2);
  for (auto& i : a) {
    i = 100;
  }
  r.get<vector<string>>(3) = {"a", "ab", "abc"};
  auto& u = r.get<untyped_tuple>(4);
  u.get<int>(0) = 10;
  u.get<string>(1) = "tuplestring";

  EXPECT_EQ(100, get<0>(t));
  EXPECT_STREQ("string", get<1>(t).str().c_str());
  auto& b = get<2>(t);
  for (auto& i : b) {
    EXPECT_EQ(100, i);
  }
  EXPECT_EQ(3, get<3>(t).size());
  EXPECT_STREQ("a", get<3>(t)[0].str().c_str());
  EXPECT_STREQ("ab", get<3>(t)[1].str().c_str());
  EXPECT_STREQ("abc", get<3>(t)[2].str().c_str());
  auto& v = get<4>(t);
  EXPECT_EQ(10, get<0>(v));
  EXPECT_STREQ("tuplestring", get<1>(v).str().c_str());
}

TEST_F(TupleTest, get_and_set2) {
  tuple<
    int32_t,
    string,
    array<int, 10>,
    vector<string>,
    tuple<int, string>> t;
  t.untyped_tuple_.reset(meta);

  get<0>(t) = 100;
  get<1>(t) = "string";
  auto& a = get<2>(t);
  for (auto& i : a) {
    i = 100;
  }
  get<3>(t) = {"a", "ab", "abc"};
  auto& u = get<4>(t);
  get<0>(u) = 10;
  get<1>(u) = "tuplestring";

  auto& r = t.untyped_tuple_;

  EXPECT_EQ(100, r.get<int32_t>(0));
  EXPECT_STREQ("string", r.get<string>(1).str().c_str());
  auto& b = r.get<array<int, 10>>(2);
  for (auto& i : b) {
    EXPECT_EQ(100, i);
  }
  EXPECT_EQ(3, r.get<vector<string>>(3).size());
  EXPECT_STREQ("a", r.get<vector<string>>(3)[0].str().c_str());
  EXPECT_STREQ("ab", r.get<vector<string>>(3)[1].str().c_str());
  EXPECT_STREQ("abc", r.get<vector<string>>(3)[2].str().c_str());
  auto& v = r.get<untyped_tuple>(4);
  EXPECT_EQ(10, v.get<int>(0));
  EXPECT_STREQ("tuplestring", v.get<string>(1).str().c_str());
}

TEST_F(TupleTest, make_tuple) {
  auto u = crystal::make_tuple(
      10,
      string("tuplestring"));
  auto t = crystal::make_tuple(
      100,
      string("string"),
      array<int, 10>({0,1,2,3,4,5,6,7,8,9}),
      vector<string>({"a", "ab", "abc"}),
      std::move(u));

  EXPECT_EQ(100, get<0>(t));
  EXPECT_STREQ("string", get<1>(t).str().c_str());
  auto& b = get<2>(t);
  int k = 0;
  for (auto& i : b) {
    EXPECT_EQ(k++, i);
  }
  EXPECT_EQ(3, get<3>(t).size());
  EXPECT_STREQ("a", get<3>(t)[0].str().c_str());
  EXPECT_STREQ("ab", get<3>(t)[1].str().c_str());
  EXPECT_STREQ("abc", get<3>(t)[2].str().c_str());
  auto& v = get<4>(t);
  EXPECT_EQ(10, get<0>(v));
  EXPECT_STREQ("tuplestring", get<1>(v).str().c_str());
}
