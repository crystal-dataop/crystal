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

#include "crystal/serializer/record/containers/Serialization.h"
#include "crystal/serializer/record/containers/SerializationInUpdating.h"
#include "crystal/serializer/record/containers/UnTypedTuple.h"
#include "crystal/serializer/record/containers/TupleMetaGenerator.h"

using namespace crystal;

TEST(untyped_tuple_meta, generate) {
  untyped_tuple::meta meta =
    generateTupleMeta<
      tuple<
        int32_t,
        string,
        array<int, 10>,
        vector<string>,
        tuple<int, string>>>();
  EXPECT_EQ(5, meta.size());
  EXPECT_EQ(77, meta.fixed_size());
  EXPECT_EQ(DataType::INT32, meta[0].type);
  EXPECT_EQ(1, meta[0].count);
  EXPECT_EQ(1, meta[0].offset);
  EXPECT_EQ(DataType::STRING, meta[1].type);
  EXPECT_EQ(1, meta[1].count);
  EXPECT_EQ(5, meta[1].offset);
  EXPECT_EQ(DataType::INT32, meta[2].type);
  EXPECT_EQ(10, meta[2].count);
  EXPECT_EQ(13, meta[2].offset);
  EXPECT_EQ(DataType::STRING, meta[3].type);
  EXPECT_EQ(0, meta[3].count);
  EXPECT_EQ(53, meta[3].offset);
  EXPECT_EQ(DataType::TUPLE, meta[4].type);
  EXPECT_EQ(1, meta[4].count);
  EXPECT_EQ(61, meta[4].offset);

  untyped_tuple::meta submeta{meta[4].submeta};
  EXPECT_EQ(2, submeta.size());
  EXPECT_EQ(13, submeta.fixed_size());
  EXPECT_EQ(DataType::INT32, submeta[0].type);
  EXPECT_EQ(1, submeta[0].count);
  EXPECT_EQ(1, submeta[0].offset);
  EXPECT_EQ(DataType::STRING, submeta[1].type);
  EXPECT_EQ(1, submeta[1].count);
  EXPECT_EQ(5, submeta[1].offset);

  meta.release();
}
