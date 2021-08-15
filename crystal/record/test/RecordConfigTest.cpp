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

#include "crystal/record/test/RecordTest.h"

using namespace crystal;

TEST_F(RecordTest, config) {
  EXPECT_EQ(4, config.fieldConfigs.size());
  EXPECT_STREQ("menuId", config.fieldConfigs[0].name().c_str());
  EXPECT_EQ(DataType::UINT64, config.fieldConfigs[0].type());
  EXPECT_STREQ("name", config.fieldConfigs[1].name().c_str());
  EXPECT_EQ(DataType::STRING, config.fieldConfigs[1].type());
  EXPECT_STREQ("food", config.fieldConfigs[2].name().c_str());
  EXPECT_EQ(DataType::TUPLE, config.fieldConfigs[2].type());
  EXPECT_EQ(4, config.fieldConfigs[2].tuple().size());
  EXPECT_STREQ("foodId", config.fieldConfigs[2].tuple()[0].name().c_str());
  EXPECT_EQ(DataType::UINT64, config.fieldConfigs[2].tuple()[0].type());
  EXPECT_STREQ("name", config.fieldConfigs[2].tuple()[1].name().c_str());
  EXPECT_EQ(DataType::STRING, config.fieldConfigs[2].tuple()[1].type());
  EXPECT_STREQ("price", config.fieldConfigs[2].tuple()[2].name().c_str());
  EXPECT_EQ(DataType::FLOAT, config.fieldConfigs[2].tuple()[2].type());
  EXPECT_STREQ("onsale", config.fieldConfigs[2].tuple()[3].name().c_str());
  EXPECT_EQ(DataType::BOOL, config.fieldConfigs[2].tuple()[3].type());
  EXPECT_STREQ("restaurantId", config.fieldConfigs[3].name().c_str());
  EXPECT_EQ(DataType::UINT64, config.fieldConfigs[3].type());
  EXPECT_STREQ("restaurant", config.fieldConfigs[3].related().c_str());

  untyped_tuple::meta meta = config.buildRecordMeta("*");
  EXPECT_EQ(4, meta.size());
  EXPECT_EQ(DataType::UINT64, meta[0].type);
  EXPECT_EQ(1, meta[0].count);
  EXPECT_EQ(DataType::STRING, meta[1].type);
  EXPECT_EQ(1, meta[1].count);
  EXPECT_EQ(DataType::TUPLE, meta[2].type);
  EXPECT_EQ(0, meta[2].count);
  untyped_tuple::meta subm{meta[2].submeta};
  EXPECT_EQ(4, subm.size());
  EXPECT_EQ(DataType::UINT64, subm[0].type);
  EXPECT_EQ(1, subm[0].count);
  EXPECT_EQ(DataType::STRING, subm[1].type);
  EXPECT_EQ(1, subm[1].count);
  EXPECT_EQ(DataType::FLOAT, subm[2].type);
  EXPECT_EQ(1, subm[2].count);
  EXPECT_EQ(DataType::BOOL, subm[3].type);
  EXPECT_EQ(1, subm[3].count);
  EXPECT_EQ(DataType::UINT64, meta[3].type);
  EXPECT_EQ(1, meta[3].count);
}
