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

#include <filesystem>
#include <gflags/gflags.h>

#include "crystal/foundation/File.h"
#include "crystal/foundation/SystemUtil.h"
#include "crystal/storage/table/TableFactory.h"

DEFINE_string(conf, "", "crystal conf");
DEFINE_string(data, "", "crystal data");
DEFINE_string(output, "", "dump output");
DEFINE_bool(use_cson, false, "use cson as input format");
DEFINE_int32(loglevel, 2, "log level: 0~4 = DIWEF");

using namespace crystal;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "Usage: " + getProcessName() +
      " -conf CONF -data DATA -output OUTPUT"
      " [-use_cson] [-loglevel N]");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_loglevel = std::min(FLAGS_loglevel, 4);
  FLAGS_loglevel = std::max(FLAGS_loglevel, -5);
  Singleton<logging::Logger>::get().setLevel(FLAGS_loglevel);

  if (FLAGS_conf.empty() || FLAGS_data.empty() || FLAGS_output.empty()) {
    CRYSTAL_LOG(ERROR) << "conf & data & output needed, see -help";
    return -1;
  }

  TableFactory factory;
  if (!factory.load(FLAGS_conf.c_str(), FLAGS_data.c_str(), true)) {
    CRYSTAL_LOG(ERROR) << "load data from '" << FLAGS_data
        << "' with conf '" << FLAGS_conf << "' failed";
    return -1;
  }

  std::filesystem::path root(FLAGS_output);
  if (std::filesystem::exists(root)) {
    CRYSTAL_LOG(ERROR) << "output '" << root << "' already exist";
    return -1;
  }
  std::filesystem::create_directories(root);

  auto& tables = factory.getTableGroup()->getTables();
  for (auto& table : tables) {
    std::filesystem::create_directory(root / table.first);
    uint16_t segCount = table.second->getKVSegmentCount();
    std::filesystem::path kvDir = root / table.first / "kv";
    if (segCount > 0) {
      std::filesystem::create_directory(kvDir);
    }
    for (uint16_t seg = 0; seg < segCount; ++seg) {
      KV* kv = table.second->getKV(seg);
      auto j = kv->serialize();
      auto s = FLAGS_use_cson ? toPrettyCson(j) : toPrettyJson(j);
      std::filesystem::path segFile = kvDir / to<std::string>(seg);
      if (!writeFile(s, segFile.c_str())) {
        CRYSTAL_LOG(ERROR)
            << "write serialized data to '" << segFile << "' failed";
        return -1;
      }
    }
  }

  return 0;
}
