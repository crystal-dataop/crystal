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

#include "crystal/record/containers/Serialization.h"
#include "crystal/record/containers/SerializationInUpdating.h"
#include "crystal/record/containers/Vector.h"

using namespace crystal;

TEST(vector, assign) {
  vector<int> vec;
  vec.assign(6, 10);
  EXPECT_EQ(6, vec.size());
  for (auto& v : vec) {
    EXPECT_EQ(10, v);
  }

  vec.assign({1,3,5,7,9});
  EXPECT_EQ(5, vec.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    EXPECT_EQ(i * 2 + 1, vec[i]);
  }
}

TEST(vector, serialize) {
  vector<int> vec(6, 100);
  EXPECT_EQ(25, bufferSize(vec));
  void* buffer = std::malloc(bufferSize(vec));
  {
    vector<int> to;
    serialize(vec, to, buffer);
    EXPECT_EQ(6, to.size());
    for (auto& v : vec) {
      EXPECT_EQ(100, v);
    }
  }
  {
    vector<int> to = vec;
    EXPECT_EQ(25, bufferSizeToUpdate(to));
    serializeInUpdating(to, buffer);
    EXPECT_EQ(0, bufferSizeToUpdate(to));
    EXPECT_EQ(6, to.size());
    for (auto& v : vec) {
      EXPECT_EQ(100, v);
    }
  }
  std::free(buffer);
}

TEST(vector, string) {
  vector<string> vec;
  vec.assign(6, "string");
  EXPECT_EQ(6, vec.size());
  for (auto& v : vec) {
    EXPECT_STREQ("string", v.str().c_str());
  }

  EXPECT_EQ(91, bufferSize(vec));
  void* buffer = std::malloc(bufferSize(vec));
  {
    vector<string> to;
    serialize(vec, to, buffer);
    EXPECT_EQ(6, to.size());
    for (auto& v : vec) {
      EXPECT_STREQ("string", v.str().c_str());
    }
  }
  {
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    vector<string> to = vec;
    EXPECT_EQ(91, bufferSizeToUpdate(to));
    serializeInUpdating(to[0], p);
    EXPECT_EQ(84, bufferSizeToUpdate(to));
    serializeInUpdating(to, p + 7);
    EXPECT_EQ(0, bufferSizeToUpdate(to));
    EXPECT_EQ(6, to.size());
    for (auto& v : vec) {
      EXPECT_STREQ("string", v.str().c_str());
    }
  }
  std::free(buffer);
}

TEST(vector, pair) {
  vector<pair<int, string>> vec;
  vec.assign(6, pair<int, string>(100, "string"));
  EXPECT_EQ(6, vec.size());
  for (auto& v : vec) {
    EXPECT_EQ(100, v.first);
    EXPECT_STREQ("string", v.second.str().c_str());
  }

  EXPECT_EQ(139, bufferSize(vec));
  void* buffer = std::malloc(bufferSize(vec));
  {
    vector<pair<int, string>> to;
    serialize(vec, to, buffer);
    EXPECT_EQ(6, to.size());
    for (auto& v : vec) {
      EXPECT_EQ(100, v.first);
      EXPECT_STREQ("string", v.second.str().c_str());
    }
  }
  {
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    vector<pair<int, string>> to = vec;
    EXPECT_EQ(139, bufferSizeToUpdate(to));
    serializeInUpdating(to[0], p);
    EXPECT_EQ(132, bufferSizeToUpdate(to));
    serializeInUpdating(to, p + 7);
    EXPECT_EQ(0, bufferSizeToUpdate(to));
    EXPECT_EQ(6, to.size());
    for (auto& v : vec) {
      EXPECT_EQ(100, v.first);
      EXPECT_STREQ("string", v.second.str().c_str());
    }
  }
  std::free(buffer);
}
