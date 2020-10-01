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

#include "crystal/dataframe/DataView.h"
#include "crystal/dataframe/test/DataFrameTest.h"

using namespace crystal;

CRYSTAL_FIELD_INDEX_META(
    FIM,
    foodId, status, name, desc, price, menuId,
    menu__menuId, menu__status, menu__content, menu__food);

TEST_F(DataFrameTest, DataView_read) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  ExtendedTable* extable = factory.getExtendedTable("restaurant/food");
  DataView view(std::make_unique<DocumentArray>(extable));

  EXPECT_EQ(10, view.getColCount());

  EXPECT_EQ(0, view.getRowCount());
  view.getBaseTable()->docs().emplace(extable, 0, 1);
  EXPECT_EQ(0, view.getRowCount());
  view.docIndex().resize(view.getBaseTable()->getDocCount());
  EXPECT_EQ(1, view.getRowCount());

  view.initFieldIndexMeta<FIM>();
  FIM& f = view.fieldIndexMeta<FIM>();

  EXPECT_EQ(100, *view.get<uint64_t>(0, f.foodId));
  EXPECT_EQ(2, *view.get<int32_t>(0, f.status));
  EXPECT_STREQ("food name", view.get<std::string>(0, f.name)->c_str());
  EXPECT_STREQ("food desc",
               std::string(*view.get<std::string_view>(0, f.desc)).c_str());
  EXPECT_FLOAT_EQ(5.5, *view.get<float>(0, f.price));
  EXPECT_EQ((1ul << 32) | 1, *view.get<uint64_t>(0, f.menuId));
  EXPECT_EQ(1000, *view.get<uint64_t>(0, f.menu__menuId));
  EXPECT_EQ(2, *view.get<int32_t>(0, f.menu__status));
  EXPECT_STREQ("menu content",
               view.get<std::string>(0, f.menu__content)->c_str());
  auto food = view.get<Array<std::string_view>>(0, f.menu__food);
  EXPECT_EQ(3, food->size());
  EXPECT_STREQ("a", std::string(food->get(0)).c_str());
  EXPECT_STREQ("b", std::string(food->get(1)).c_str());
  EXPECT_STREQ("c", std::string(food->get(2)).c_str());
}
