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

#include "crystal/storage/builder/RecordBuilder.h"

using namespace crystal;

TEST(RecordBuilder, record) {
  const char* conf = R"(
      {
        record=[
          { tag=1, name="menuId", type="uint64" },
          { tag=2, name="status", type="int32", bits=4, default=1 },
          { tag=3, name="keyword", type="string" },
          { tag=4, name="content", type="string" },
          { tag=5, name="food", type="string", count=0 }
        ]
      }
      )";
  const char* cson = R"(
      {
        menuId=1000,
        status=0,
        keyword="menu",
        content="menu content",
        food=["a","b","c"]
      }
      )";

  RecordBuilder<SysAllocator> builder(parseCson(conf));
  builder.init(nullptr);

  Record record = builder.build(parseCson(cson));

  EXPECT_EQ(1000, record.get<uint64_t>(1));
  EXPECT_EQ(0, record.get<int32_t>(2));
  EXPECT_STREQ("menu", record.get<std::string>(3).c_str());
  EXPECT_STREQ("menu content", record.get<std::string>(4).c_str());
  Array<std::string_view> a = record.get<Array<std::string_view>>(5);
  EXPECT_EQ(3, a.size());
  EXPECT_STREQ("a", std::string(a.get(0)).c_str());
  EXPECT_STREQ("b", std::string(a.get(1)).c_str());
  EXPECT_STREQ("c", std::string(a.get(2)).c_str());
}
