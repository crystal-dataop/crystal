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

#include "crystal/dataframe/Document.h"
#include "crystal/dataframe/test/DataFrameTest.h"

using namespace crystal;

TEST_F(DataFrameTest, Document_read) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  ExtendedTable* extable = factory.getExtendedTable("restaurant/food");

  Document doc(extable, 0, 1);

  EXPECT_TRUE(!!doc);
  EXPECT_EQ(1, doc.id());

  EXPECT_EQ(100, *doc.get<uint64_t>(1));
  EXPECT_EQ((1ul << 32) | 1, *doc.get<uint64_t>(2));
  EXPECT_EQ(1000, *doc.get<uint64_t>(8));
}
