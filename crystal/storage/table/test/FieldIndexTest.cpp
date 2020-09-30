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

#include "crystal/storage/table/FieldIndex.h"

using namespace crystal;

TEST(FieldIndex, all) {
  FieldIndex index;

  index.build(std::vector<std::string>({"a", "b", "c"}));
  EXPECT_EQ(3, index.size());
  EXPECT_EQ(0, index.getIndexOfField("a"));
  EXPECT_EQ(1, index.getIndexOfField("b"));
  EXPECT_EQ(2, index.getIndexOfField("c"));

  index.append("d");
  EXPECT_EQ(4, index.size());
  EXPECT_EQ(3, index.getIndexOfField("d"));

  std::vector<std::string> fields = {"a", "c", "e"};
  index.select(fields);
  EXPECT_EQ(0, index.getIndexOfField("a"));
  EXPECT_EQ(npos, index.getIndexOfField("b"));
  EXPECT_EQ(2, index.getIndexOfField("c"));
  EXPECT_EQ(npos, index.getIndexOfField("d"));

  EXPECT_EQ(1, fields.size());
  EXPECT_STREQ("e", fields[0].c_str());

  index.append("e");
  EXPECT_EQ(3, index.size());
  EXPECT_EQ(4, index.getIndexOfField("e"));
}
