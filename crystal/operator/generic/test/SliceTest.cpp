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

#include "crystal/operator/generic/Slice.h"
#include "crystal/operator/search/Search.h"
#include "crystal/operator/test/OperatorTest.h"

using namespace crystal;
using namespace crystal::op;

TEST_F(OperatorTest, Slice) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  ExtendedTable* extable = factory.getExtendedTable("restaurant/food");
  DataView view(std::make_unique<DocumentArray>(extable));

  std::vector<uint64_t> tokens = {1,2,3,4,5,6,7};
  view | search(tokens);
  EXPECT_EQ(7, view.getRowCount());

  FieldKey key("foodId");

  view
    | slice(2, 4, true)
    | [&](DataView& view) {
        // 1,2,5,6,7
        EXPECT_EQ(5, view.getRowCount());
        EXPECT_EQ(1, *view.get<uint64_t>(0, key));
        EXPECT_EQ(2, *view.get<uint64_t>(1, key));
        EXPECT_EQ(5, *view.get<uint64_t>(2, key));
        EXPECT_EQ(6, *view.get<uint64_t>(3, key));
        EXPECT_EQ(7, *view.get<uint64_t>(4, key));
      }
    | slice(std::vector<size_t>({1}), true)
    | [&](DataView& view) {
        // 1,5,6,7
        EXPECT_EQ(4, view.getRowCount());
        EXPECT_EQ(1, *view.get<uint64_t>(0, key));
        EXPECT_EQ(5, *view.get<uint64_t>(1, key));
        EXPECT_EQ(6, *view.get<uint64_t>(2, key));
        EXPECT_EQ(7, *view.get<uint64_t>(3, key));
      }
    | slice(2)
    | [&](DataView& view) {
        // 6,7
        EXPECT_EQ(2, view.getRowCount());
        EXPECT_EQ(6, *view.get<uint64_t>(0, key));
        EXPECT_EQ(7, *view.get<uint64_t>(1, key));
      }
    | slice(std::vector<size_t>({1}))
    | [&](DataView& view) {
        // 7
        EXPECT_EQ(1, view.getRowCount());
        EXPECT_EQ(7, *view.get<uint64_t>(0, key));
      };
}
