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

#pragma once

#include <filesystem>
#include <gtest/gtest.h>

#include "crystal/foundation/SystemUtil.h"

namespace crystal {

class TableFactoryTest : public ::testing::Test {
 protected:
  std::string path = getProcessName() + "_data";
  std::filesystem::path conf =
    std::filesystem::path(__FILE__).parent_path() / "tablegroup.cson";

  void SetUp() override {
    static bool sOnce = true;
    if (sOnce) {
      std::filesystem::remove_all(path);
      sOnce = false;
    }
  }
};

} // namespace crystal
