/*
 * Copyright 2020 Yeolar
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

#include "crystal/type/DataType.h"

using namespace crystal;

TEST(DataType, all) {
  EXPECT_EQ(1, sizeOf(DataType::BOOL));
  EXPECT_EQ(1, sizeOf(DataType::INT8));
  EXPECT_EQ(4, sizeOf(DataType::FLOAT));
  EXPECT_EQ(8, sizeOf(DataType::DOUBLE));
  EXPECT_EQ(8, sizeOf(DataType::STRING));

  EXPECT_TRUE(checkType<bool>(DataType::BOOL));
  EXPECT_TRUE(checkType<int8_t>(DataType::INT8));
  EXPECT_TRUE(checkType<float>(DataType::FLOAT));
  EXPECT_TRUE(checkType<double>(DataType::DOUBLE));
  EXPECT_TRUE(checkType<std::string_view>(DataType::STRING));
}
