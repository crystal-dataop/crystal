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

#include "crystal/storage/table/ExtendedTable.h"
#include "crystal/storage/table/test/TableGroupTest.h"

using namespace crystal;

TEST_F(TableGroupTest, ExtendedTable_all) {
  TableGroupConfig config;
  dynamic j = parseCson(conf);
  EXPECT_TRUE(config.parse(j));

  TableGroup tablegroup(config);
  EXPECT_TRUE(tablegroup.init(path.c_str(), false));

  Table* table = tablegroup.getTable("food");

  ExtendedTable extable(table, &tablegroup);

  EXPECT_STREQ("food", extable.name().c_str());
  EXPECT_STREQ("foodId", extable.key().c_str());
  EXPECT_EQ(10, extable.fields().size());
  EXPECT_STREQ("desc", extable.fields()[0].c_str());
  EXPECT_STREQ("foodId", extable.fields()[1].c_str());
  EXPECT_STREQ("menuId", extable.fields()[2].c_str());
  EXPECT_STREQ("name", extable.fields()[3].c_str());
  EXPECT_STREQ("price", extable.fields()[4].c_str());
  EXPECT_STREQ("status", extable.fields()[5].c_str());
  EXPECT_STREQ("menu__content", extable.fields()[6].c_str());
  EXPECT_STREQ("menu__food", extable.fields()[7].c_str());
  EXPECT_STREQ("menu__menuId", extable.fields()[8].c_str());
  EXPECT_STREQ("menu__status", extable.fields()[9].c_str());
  EXPECT_TRUE(extable.hasKV());
}
