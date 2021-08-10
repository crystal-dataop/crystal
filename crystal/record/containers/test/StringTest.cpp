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
#include "crystal/record/containers/String.h"

using namespace crystal;

TEST(string, assign) {
  string str;
  str.assign("string");
  EXPECT_EQ(6, str.size());
  EXPECT_STREQ("string", str.str().c_str());

  str.assign("another string");
  EXPECT_EQ(14, str.size());
  EXPECT_STREQ("another string", str.str().c_str());
}

TEST(string, serialize) {
  string str("string");
  EXPECT_EQ(7, bufferSize(str));
  void* buffer = std::malloc(bufferSize(str));
  {
    string to;
    serialize(str, to, buffer);
    EXPECT_EQ(6, to.size());
    EXPECT_STREQ("string", to.str().c_str());
    EXPECT_EQ(0, bufferSizeToUpdate(to));
  }
  {
    string to = str;
    EXPECT_EQ(7, bufferSizeToUpdate(to));
    serializeInUpdating(to, buffer);
    EXPECT_EQ(6, to.size());
    EXPECT_STREQ("string", to.str().c_str());
    EXPECT_EQ(0, bufferSizeToUpdate(to));
  }
  std::free(buffer);
}
