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

#include <gflags/gflags.h>

#include "crystal/foundation/File.h"
#include "crystal/foundation/SystemUtil.h"
#include "crystal/operator/generic/Serialize.h"
#include "crystal/query/Query.h"

DEFINE_string(conf, "", "crystal conf");
DEFINE_string(data, "", "crystal data");
DEFINE_string(input, "", "query input, from stdin if empty");
DEFINE_string(output, "", "result output, to stdout if empty");
DEFINE_bool(use_cson, false, "use cson as input&output format");
DEFINE_int32(loglevel, 2, "log level: 0~4 = DIWEF");

using namespace crystal;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "Usage: " + getProcessName() +
      " -conf CONF -data DATA [-input QUERY] [-output RESULT]"
      " [-use_cson] [-loglevel N]");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_loglevel = std::min(FLAGS_loglevel, 4);
  FLAGS_loglevel = std::max(FLAGS_loglevel, -5);
  Singleton<logging::Logger>::get().setLevel(FLAGS_loglevel);

  if (FLAGS_conf.empty() || FLAGS_data.empty()) {
    CRYSTAL_LOG(ERROR) << "conf & data needed, see -help";
    return -1;
  }

  TableFactory factory;
  if (!factory.load(FLAGS_conf.c_str(), FLAGS_data.c_str(), true)) {
    CRYSTAL_LOG(ERROR) << "load data from '" << FLAGS_data
        << "' with conf '" << FLAGS_conf << "' failed";
    return -1;
  }

  Graph::Executor executor;
  if (FLAGS_input.empty()) {
    if (!FLAGS_output.empty()) {
      CRYSTAL_LOG(WARN) << "output to stdout if query input from stdin";
      FLAGS_output = "";
    }
    while (true) {
      CRYSTAL_RLOG(FATAL) << "please enter your query:";
      Query query(&factory, &executor, FLAGS_use_cson);
      char line[LINE_MAX] = {0};
      while (fgets(line, LINE_MAX, stdin)) {
        std::string_view sv(line, strlen(line));
        sv = trimWhitespace(sv);
        if (sv == "") {
          break;
        }
        query += std::string(sv);
      }
      if (query.query().empty()) {
        continue;
      }
      DataView view = query.run();
      std::string result = FLAGS_use_cson
          ? (view | op::toCson(true, true))
          : (view | op::toJson(true, true));
      CRYSTAL_RLOG(FATAL) << result;
    }
  } else {
    std::string queryStr;
    if (!readFile(FLAGS_input.c_str(), queryStr)) {
      CRYSTAL_LOG(ERROR) << "read query input '" << FLAGS_input << "' failed";
      return -1;
    }
    Query query(&factory, &executor, FLAGS_use_cson);
    std::string_view sv(queryStr);
    sv = trimWhitespace(sv);
    query += std::string(sv);
    if (query.query().empty()) {
      CRYSTAL_LOG(ERROR) << "empty query from input '" << FLAGS_input << "'";
      return -1;
    }
    DataView view = query.run();
    std::string result = FLAGS_use_cson
        ? (view | op::toCson(true, true))
        : (view | op::toJson(true, true));
    if (!FLAGS_output.empty()) {
      if (!writeFile(result, FLAGS_output.c_str())) {
        CRYSTAL_LOG(ERROR) << "write result to '" << FLAGS_output << "' failed";
        return -1;
      }
    } else {
      CRYSTAL_RLOG(FATAL) << result;
    }
  }

  return 0;
}
