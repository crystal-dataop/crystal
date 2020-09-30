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

#include "crystal/dataframe/DynamicTable.h"

using namespace crystal;

TEST(DynamicTable, Item) {
  Item item;

  int v = 100;
  EXPECT_FALSE(item.get<int>());
  item.set(v);
  EXPECT_EQ(v, *item.get<int>());
}

TEST(DynamicTable, Column) {
  Column column;

  for (size_t i = 0; i < 1000; ++i) {
    column.set(i, int(i));
    EXPECT_EQ(int(i), *column.get<int>(i));
  }
}

TEST(DynamicTable, DynamicTable) {
  DynamicTable table;

  table.appendField("fieldA");
  table.appendField("fieldB");
  table.appendField("fieldC", true);

  EXPECT_FALSE(table.isColSet(0));
  EXPECT_FALSE(table.isColSet(1));
  EXPECT_FALSE(table.isColSet(2));

  for (size_t i = 0; i < 1000; ++i) {
    EXPECT_TRUE(table.set(i, 0, int(i)));
    EXPECT_TRUE(table.set(i, 1, float(i * 0.1)));
    EXPECT_TRUE(table.set(i, 2, to<std::string>(i)));
  }

  EXPECT_TRUE(table.isColSet(0));
  EXPECT_TRUE(table.isColSet(1));
  EXPECT_TRUE(table.isColSet(2));
  EXPECT_EQ(DataType::INT32, table.getColType(0).type);
  EXPECT_EQ(DataType::FLOAT, table.getColType(1).type);
  EXPECT_EQ(DataType::STRING, table.getColType(2).type);
  EXPECT_EQ(1000, table.getDocCount());

  for (size_t i = 0; i < 1000; ++i) {
    EXPECT_EQ(i, *table.get<int>(i, 0));
    EXPECT_FLOAT_EQ(i * 0.1, *table.get<float>(i, 1));
    EXPECT_STREQ(to<std::string>(i).c_str(),
                 table.get<std::string>(i, 2)->c_str());
  }
}
