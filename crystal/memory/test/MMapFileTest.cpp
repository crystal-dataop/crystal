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

#include "crystal/foundation/File.h"
#include "crystal/memory/MMapFile.h"

using namespace crystal;

TEST(MMapFile, all) {
  MMapFile mmapFile(File::temporary());

  EXPECT_EQ(nullptr, mmapFile.get());
  EXPECT_EQ(0, mmapFile.size());

  EXPECT_TRUE(mmapFile.init(1024, PROT_READ | PROT_WRITE));

  EXPECT_NE(nullptr, mmapFile.get());
  EXPECT_EQ(1024, mmapFile.size());
  EXPECT_EQ(0, mmapFile.file().size());

  float v = 0;
  writeFull(mmapFile.file().fd(), &v, sizeof(v));
  EXPECT_EQ(4, mmapFile.file().size());

  EXPECT_TRUE(mmapFile.sync(4));
  EXPECT_EQ(4, mmapFile.file().size());
}
