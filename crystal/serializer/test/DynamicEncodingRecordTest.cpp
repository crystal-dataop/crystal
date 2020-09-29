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

#include "crystal/memory/SysAllocator.h"
#include "crystal/serializer/DynamicEncoding.h"
#include "crystal/serializer/record/RecordConfig.h"

using namespace crystal;

TEST(DynamicEncoding, record) {
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
  const char* json = R"(
      {
        "menuId": 1000,
        "status": 0,
        "keyword": "menu",
        "content": "menu content",
        "food": ["a","b","c"]
      }
      )";

  RecordMeta meta = buildRecordMeta(parseRecordConfig(parseCson(conf)));

  Accessor accessor(meta);
  SysAllocator allocator;
  void* buf = allocator.address(allocator.allocate(accessor.bufferSize()));
  memset(buf, 0, accessor.bufferSize());

  Record record;
  record.init(&meta, &accessor, &allocator, buf);

  record.reset();
  decode(parseCson(cson), record);

  EXPECT_EQ(1000, record.get<uint64_t>(*meta.getMeta(1)));
  EXPECT_EQ(0, record.get<int32_t>(*meta.getMeta(2)));
  EXPECT_STREQ("menu", record.get<std::string>(*meta.getMeta(3)).c_str());
  EXPECT_STREQ("menu content", record.get<std::string>(*meta.getMeta(4)).c_str());
  Array<std::string_view> a = record.get<Array<std::string_view>>(*meta.getMeta(5));
  EXPECT_EQ(3, a.size());
  EXPECT_STREQ("a", std::string(a.get(0)).c_str());
  EXPECT_STREQ("b", std::string(a.get(1)).c_str());
  EXPECT_STREQ("c", std::string(a.get(2)).c_str());

  record.reset();
  decode(parseJson(json), record);

  EXPECT_EQ(1000, record.get<uint64_t>(*meta.getMeta(1)));
  EXPECT_EQ(0, record.get<int32_t>(*meta.getMeta(2)));
  EXPECT_STREQ("menu", record.get<std::string>(*meta.getMeta(3)).c_str());
  EXPECT_STREQ("menu content", record.get<std::string>(*meta.getMeta(4)).c_str());
  Array<std::string_view> b = record.get<Array<std::string_view>>(*meta.getMeta(5));
  EXPECT_EQ(3, b.size());
  EXPECT_STREQ("a", std::string(b.get(0)).c_str());
  EXPECT_STREQ("b", std::string(b.get(1)).c_str());
  EXPECT_STREQ("c", std::string(b.get(2)).c_str());
}
