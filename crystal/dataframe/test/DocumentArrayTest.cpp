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

#include "crystal/dataframe/DocumentArray.h"
#include "crystal/dataframe/test/DataFrameTest.h"

using namespace crystal;

TEST_F(DataFrameTest, DocumentArray_read) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  ExtendedTable* extable = factory.getExtendedTable("restaurant/food");
  DocumentArray docarray(extable);

  EXPECT_EQ(10, docarray.getFieldCount());

  EXPECT_EQ(0, docarray.getDocCount());
  docarray.docs().emplace(extable, 0, 1);
  EXPECT_EQ(1, docarray.getDocCount());

  EXPECT_EQ(100, *docarray.get<uint64_t>(0, 1));
  EXPECT_EQ((1ul << 32) | 1, *docarray.get<uint64_t>(0, 2));
  EXPECT_EQ(1000, *docarray.get<uint64_t>(0, 8));
}
