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

#include "crystal/storage/kv/KVConfig.h"

using namespace crystal;

TEST(KVConfig, all) {
  KVConfig config("menu");
  dynamic j = parseCson(R"(
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
      bucket=10000
    }
  )");
  config.parse(j, parseRecordConfig(j));

  EXPECT_STREQ("menu", config.name().c_str());
  EXPECT_STREQ("menuId", config.key().c_str());
  EXPECT_EQ(DataType::UINT64, config.keyConfig().type());
  EXPECT_EQ(4, config.fields().size());
  EXPECT_EQ(10000, config.bucket());
}
