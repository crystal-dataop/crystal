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

#include "crystal/storage/table/TableFactory.h"
#include "crystal/storage/table/test/TableFactoryTest.h"
#include "crystal/strategy/Hash.h"

using namespace crystal;

TEST_F(TableFactoryTest, TableFactory_write) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), false));

  TableGroupBuilder* builder = factory.getTableGroupBuilder("restaurant");
  {
    const char* cson = R"(
        {
          menuId=1000,
          status=2,
          keyword="menu",
          content="menu content",
          food=["a","b","c"]
        }
        )";
    EXPECT_TRUE(builder->add("menu._", parseCson(cson)));
  }
  {
    const char* cson = R"(
        {
          foodId=100,
          status=2,
          name="food name",
          desc="food desc",
          price=5.5,
          menuId=1000
        }
        )";
    EXPECT_TRUE(builder->add("food._", parseCson(cson)));
  }
  factory.dump();
}

TEST_F(TableFactoryTest, TableFactory_read) {
  TableFactory factory;
  EXPECT_TRUE(factory.load(conf.c_str(), path.c_str(), true));

  TableGroup* tablegroup = factory.getTableGroup("restaurant");
  {
    Table* table = tablegroup->getTable("menu");
    EXPECT_EQ((1ul << 32) | 1, table->find(hashToken<uint64_t>(1000)));
  }
  {
    Table* table = tablegroup->getTable("food");
    EXPECT_EQ(1, table->find(hashToken<uint64_t>(100)));
  }
}
