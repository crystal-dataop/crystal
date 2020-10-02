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

DEFINE_string(conf, "", "crystal conf file");
DEFINE_string(input, "", "source data input");
DEFINE_string(output, "", "crystal build data output");
DEFINE_bool(use_cson, false, "use cson as input format");
DEFINE_int32(loglevel, 2, "log level: 0~4 = DIWEF");

using namespace crystal;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "Usage: " + getProcessName() +
      " -conf CONF -input INPUT -output OUTPUT"
      " [-use_cson] [-loglevel N]");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_loglevel = std::min(FLAGS_loglevel, 4);
  FLAGS_loglevel = std::max(FLAGS_loglevel, -5);
  Singleton<logging::Logger>::get().setLevel(FLAGS_loglevel);

  if (FLAGS_conf.empty() || FLAGS_input.empty() || FLAGS_output.empty()) {
    CRYSTAL_LOG(ERROR) << "conf & input & output needed, see -help";
    return -1;
  }

  TableFactory factory;
  if (!factory.load(FLAGS_conf.c_str(), FLAGS_output.c_str(), false)) {
    CRYSTAL_LOG(ERROR) << "table factory load '" << FLAGS_conf << "' failed";
    return -1;
  }

  TableGroupBuilder* builder = factory.getTableGroupBuilder();
  if (!builder) {
    CRYSTAL_LOG(ERROR) << "get table group builder failed";
    return -1;
  }

  CRYSTAL_RLOG(WARN) << "init table group builder:";
  CRYSTAL_RLOG(WARN) << "  output: " << FLAGS_output;
  CRYSTAL_RLOG(WARN) << "    conf: " << FLAGS_conf;

  for (auto& dir : std::filesystem::directory_iterator(FLAGS_input)) {
    std::string table = dir.path().filename();
    if (table.find('.') == table.npos) {
      table += ".*";
    }
    CRYSTAL_RLOG(WARN) << "build source data for table: " << table;
    for (auto& p : std::filesystem::directory_iterator(dir)) {
      auto file = p.path();
      std::string data;
      if (!readFile(file.c_str(), data)) {
        CRYSTAL_LOG(ERROR) << "read source data '" << file << "' failed";
        return -1;
      }
      CRYSTAL_RLOG(WARN) << "  " << file;
      for (auto& record : (FLAGS_use_cson ? parseCson(data)
                                          : parseJson(data))) {
        if (!builder->add(table, record)) {
          CRYSTAL_LOG(ERROR) << "build record "
              << table << ":" << record << " failed";
          return -1;
        }
      }
    }
  }
  factory.dump();

  return 0;
}
