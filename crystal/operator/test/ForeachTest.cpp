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

#include "crystal/operator/search/Search.h"
#include "crystal/operator/test/OperatorTest.h"

using namespace crystal;
using namespace crystal::op;

TEST_F(OperatorTest, Foreach) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  ExtendedTable* extable = factory.getExtendedTable("restaurant/food");
  DataView view(std::make_unique<DocumentArray>(extable));

  std::vector<uint64_t> tokens = {1,2,3,4,5,6,7};
  FieldKey key("foodId");

  view | search(tokens) | [&](DataView& view, uint32_t i) {
    EXPECT_EQ(i + 1, *view.get<uint64_t>(i, key.getIndex(view)));
  };
}
