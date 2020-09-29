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

#include "crystal/storage/index/IndexConfig.h"

using namespace crystal;

class IndexConfigTest : public ::testing::Test {
 protected:
  const dynamic j = parseCson(R"(
    {
      record=[
        { tag=1, name="menuId", type="uint64" },
        { tag=2, name="status", type="int32", bits=4, default=1 },
        { tag=3, name="keyword", type="string" },
        { tag=4, name="content", type="string" },
        { tag=5, name="food", type="string", count=0 }
      ],
      key="menuId",
      value=["menuId", "status", "content", "food"],
      bucket=10000,
      index=[
        {
          type="bitmap",
          key="status"
        }
      ]
    }
  )");
};

TEST_F(IndexConfigTest, bitmap) {
  IndexConfig config;
  config.parse(j["index"][0], parseRecordConfig(j));

  EXPECT_STREQ("bitmap", config.type().c_str());
  EXPECT_STREQ("status", config.key().c_str());
  EXPECT_EQ(0, config.fields().size());
}
