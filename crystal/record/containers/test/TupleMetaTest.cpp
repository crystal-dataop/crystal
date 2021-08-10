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
#include <cstdlib>

#include "crystal/record/containers/Serialization.h"
#include "crystal/record/containers/SerializationInUpdating.h"
#include "crystal/record/containers/TupleMetaGenerator.h"
#include "crystal/record/containers/UnTypedTuple.h"

using namespace crystal;

class TupleMetaTest : public ::testing::Test {
 protected:
  untyped_tuple::meta meta =
    generateTupleMeta<
      tuple<
        int32_t,
        string,
        array<int, 10>,
        vector<string>,
        tuple<int, string>>>();

  void Expect(untyped_tuple::meta& m) {
    EXPECT_EQ(5, m.size());
    EXPECT_EQ(128, m.fixed_size());
    EXPECT_EQ(77, m.tuple_fixed_size());
    EXPECT_EQ(DataType::INT32, m[0].type);
    EXPECT_EQ(1, m[0].count);
    EXPECT_EQ(1, m[0].offset);
    EXPECT_EQ(DataType::STRING, m[1].type);
    EXPECT_EQ(1, m[1].count);
    EXPECT_EQ(5, m[1].offset);
    EXPECT_EQ(DataType::INT32, m[2].type);
    EXPECT_EQ(10, m[2].count);
    EXPECT_EQ(13, m[2].offset);
    EXPECT_EQ(DataType::STRING, m[3].type);
    EXPECT_EQ(0, m[3].count);
    EXPECT_EQ(53, m[3].offset);
    EXPECT_EQ(DataType::TUPLE, m[4].type);
    EXPECT_EQ(1, m[4].count);
    EXPECT_EQ(61, m[4].offset);

    untyped_tuple::meta subm{m[4].submeta};
    EXPECT_EQ(2, subm.size());
    EXPECT_EQ(56, subm.fixed_size());
    EXPECT_EQ(13, subm.tuple_fixed_size());
    EXPECT_EQ(DataType::INT32, subm[0].type);
    EXPECT_EQ(1, subm[0].count);
    EXPECT_EQ(1, subm[0].offset);
    EXPECT_EQ(DataType::STRING, subm[1].type);
    EXPECT_EQ(1, subm[1].count);
    EXPECT_EQ(5, subm[1].offset);
  }
};

TEST_F(TupleMetaTest, generate) {
  Expect(meta);
}

TEST_F(TupleMetaTest, serialize) {
  EXPECT_EQ(184, bufferSize(meta));
  void* buffer = std::malloc(bufferSize(meta));
  {
    untyped_tuple::meta to;
    serialize(meta, to, buffer);
    Expect(to);
    to.release();
  }
  std::free(buffer);
}

TEST_F(TupleMetaTest, serializeInUpdating) {
  EXPECT_EQ(184, bufferSizeToUpdate(meta));
  void* buffer = std::malloc(bufferSize(meta));
  {
    uint8_t* p = reinterpret_cast<uint8_t*>(buffer);
    untyped_tuple::meta& submeta =
      *reinterpret_cast<untyped_tuple::meta*>(&meta[4].submeta);
    serializeInUpdating(submeta, p);
    EXPECT_EQ(128, bufferSizeToUpdate(meta));
    serializeInUpdating(meta, p + 56);
    EXPECT_EQ(0, bufferSizeToUpdate(meta));
    Expect(meta);
    meta.release();
  }
  std::free(buffer);
}
