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
#include "crystal/storage/index/bitmap/BitmapPosting.h"

using namespace crystal;

TEST(BitmapPosting, all) {
  const char* conf = R"(
      {
        record=[
          { tag=1, name="menuId", type="uint64" }
        ]
      }
      )";
  const char* cson = R"(
      {
        menuId=1000
      }
      )";

  BitmapPosting posting;

  EXPECT_EQ(8, posting.size());
  EXPECT_EQ(uint64_t(-1), posting.id);

  RecordBuilder<SysAllocator> builder(parseCson(conf), true);
  builder.init(nullptr);

  Record record = builder.build();
  decode(parseCson(cson), record);

  record.set<uint64_t>("__id", 10);
  EXPECT_TRUE(posting.parseFrom(record));
  EXPECT_EQ(10, posting.id);
}
