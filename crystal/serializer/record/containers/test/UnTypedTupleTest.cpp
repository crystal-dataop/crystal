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
#include <cstdlib>

#include "crystal/serializer/record/containers/Serialization.h"
#include "crystal/serializer/record/containers/SerializationInUpdating.h"
#include "crystal/serializer/record/containers/UnTypedTuple.h"
#include "crystal/serializer/record/containers/TupleMetaGenerator.h"

using namespace crystal;

class UnTypedTupleTest : public ::testing::Test {
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

TEST_F(UnTypedTupleTest, set_and_get) {
  untyped_tuple t(meta);
  EXPECT_EQ(5, t.size());

  EXPECT_EQ(0, t.element_buffer_size(0));
  EXPECT_EQ(0, t.get<int32_t>(0));
  t.get<int32_t>(0) = 100;
  EXPECT_EQ(100, t.get<int32_t>(0));
  EXPECT_EQ(0, t.element_buffer_size(0));

  EXPECT_EQ(0, t.element_buffer_size(1));
  EXPECT_TRUE(t.get<string>(1).empty());
  t.get<string>(1) = "string";
  EXPECT_STREQ("string", t.get<string>(1).str().c_str());
  EXPECT_EQ(7, t.element_buffer_size(1));

  EXPECT_EQ(0, t.element_buffer_size(2));
  auto& a = t.get<array<int, 10>>(2);
  for (auto& i : a) {
    EXPECT_EQ(0, i);
    i = 100;
    EXPECT_EQ(100, i);
  }
  EXPECT_EQ(0, t.element_buffer_size(2));

  EXPECT_EQ(0, t.element_buffer_size(3));
  EXPECT_TRUE(t.get<vector<string>>(3).empty());
  t.get<vector<string>>(3) = {"a", "ab", "abc"};
  EXPECT_EQ(3, t.get<vector<string>>(3).size());
  EXPECT_STREQ("a", t.get<vector<string>>(3)[0].str().c_str());
  EXPECT_STREQ("ab", t.get<vector<string>>(3)[1].str().c_str());
  EXPECT_STREQ("abc", t.get<vector<string>>(3)[2].str().c_str());
  EXPECT_EQ(34, t.element_buffer_size(3));

  EXPECT_EQ(13, t.element_buffer_size(4));
  auto& u = t.get<untyped_tuple>(4);
  EXPECT_EQ(2, u.size());

  EXPECT_EQ(0, u.get<int>(0));
  u.get<int>(0) = 10;
  EXPECT_EQ(10, u.get<int>(0));

  EXPECT_TRUE(u.get<string>(1).empty());
  u.get<string>(1) = "tuplestring";
  EXPECT_STREQ("tuplestring", u.get<string>(1).str().c_str());
  EXPECT_EQ(25, t.element_buffer_size(4));
}

TEST_F(UnTypedTupleTest, copy) {
  untyped_tuple t(meta);

  t.get<int32_t>(0) = 100;
  t.get<string>(1) = "string";
  auto& a = t.get<array<int, 10>>(2);
  for (auto& i : a) {
    i = 100;
  }
  t.get<vector<string>>(3) = {"a", "ab", "abc"};
  auto& u = t.get<untyped_tuple>(4);
  u.get<int>(0) = 10;
  u.get<string>(1) = "tuplestring";

  untyped_tuple r = t;

  EXPECT_EQ(100, r.get<int32_t>(0));
  EXPECT_STREQ("string", r.get<string>(1).str().c_str());
  auto& b = t.get<array<int, 10>>(2);
  for (auto& i : b) {
    EXPECT_EQ(100, i);
  }
  EXPECT_EQ(3, r.get<vector<string>>(3).size());
  EXPECT_STREQ("a", r.get<vector<string>>(3)[0].str().c_str());
  EXPECT_STREQ("ab", r.get<vector<string>>(3)[1].str().c_str());
  EXPECT_STREQ("abc", r.get<vector<string>>(3)[2].str().c_str());
  auto& v = t.get<untyped_tuple>(4);
  EXPECT_EQ(10, v.get<int>(0));
  EXPECT_STREQ("tuplestring", v.get<string>(1).str().c_str());
}

TEST_F(UnTypedTupleTest, serialize) {
  untyped_tuple t(meta);

  t.get<int32_t>(0) = 100;
  t.get<string>(1) = "string";
  auto& a = t.get<array<int, 10>>(2);
  for (auto& i : a) {
    i = 100;
  }
  t.get<vector<string>>(3) = {"a", "ab", "abc"};
  auto& u = t.get<untyped_tuple>(4);
  u.get<int>(0) = 10;
  u.get<string>(1) = "tuplestring";

  EXPECT_EQ(143, bufferSize(t));
  void* buffer = std::malloc(bufferSize(t));
  {
    untyped_tuple r(meta);
    serialize(t, r, buffer);

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
  std::free(buffer);
}

TEST_F(UnTypedTupleTest, serializeInUpdating) {
  untyped_tuple t(meta);

  t.get<int32_t>(0) = 100;
  t.get<string>(1) = "string";
  auto& a = t.get<array<int, 10>>(2);
  for (auto& i : a) {
    i = 100;
  }
  t.get<vector<string>>(3) = {"a", "ab", "abc"};
  auto& u = t.get<untyped_tuple>(4);
  u.get<int>(0) = 10;
  u.get<string>(1) = "tuplestring";

  EXPECT_EQ(143, bufferSizeToUpdate(t));
  void* buffer = std::malloc(bufferSizeToUpdate(t));
  {
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    untyped_tuple r = t;
    serializeInUpdating(r.get<string>(1), p);
    EXPECT_EQ(136, bufferSizeToUpdate(r));
    serializeInUpdating(r.get<array<int, 10>>(2), p + 7);
    EXPECT_EQ(136, bufferSizeToUpdate(r));
    serializeInUpdating(r.get<vector<string>>(3), p + 7);
    EXPECT_EQ(102, bufferSizeToUpdate(r));
    serializeInUpdating(r.get<untyped_tuple>(4).get<string>(1), p + 41);
    EXPECT_EQ(90, bufferSizeToUpdate(r));
    serializeInUpdating(r, p + 53);
    EXPECT_EQ(0, bufferSizeToUpdate(r));

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
  std::free(buffer);
}
