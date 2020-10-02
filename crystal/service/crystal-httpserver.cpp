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
#include <memory>
#include <stdexcept>

#include "crystal/foundation/File.h"
#include "crystal/foundation/SystemUtil.h"
#include "crystal/foundation/http/HttpServer.h"
#include "crystal/operator/generic/Serialize.h"
#include "crystal/query/Query.h"

DEFINE_string(conf, "", "crystal conf");
DEFINE_string(data, "", "crystal data");
DEFINE_bool(use_cson, false, "use cson as input&output format");
DEFINE_int32(loglevel, 2, "log level: 0~4 = DIWEF");
DEFINE_uint32(port, 80, "http server port");

using namespace crystal;

int main(int argc, char** argv) {
  gflags::SetUsageMessage(
      "Usage: " + getProcessName() +
      " -conf CONF -data DATA [-use_cson] [-loglevel N] [-port 80]");
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

  HttpServer server;
  server.config.port = FLAGS_port;

  Graph::Executor executor;

  server.default_resource["POST"] =
      [&](std::shared_ptr<HttpServer::Response> response,
         std::shared_ptr<HttpServer::Request> request) {
        Query query(&factory, &executor, FLAGS_use_cson);

        std::string queryStr = request->content.string();
        std::string_view sv(queryStr);
        sv = trimWhitespace(sv);
        query += std::string(sv);
        if (query.query().empty()) {
          response->write(StatusCode::client_error_bad_request, "empty query");
          return;
        }

        try {
          DataView view = query.run();
          std::string result = FLAGS_use_cson
              ? (view | op::toCson(true, true))
              : (view | op::toJson(true, true));
          response->write(result);
        } catch (const std::exception& e) {
          response->write(StatusCode::server_error_internal_server_error,
                          e.what());
        }
      };

  std::promise<unsigned short> serverPort;
  std::thread serverThread([&server, &serverPort]() {
    server.start(
        [&serverPort](unsigned short port) { serverPort.set_value(port); });
  });
  CRYSTAL_LOG(INFO) << "Crystal HTTPServer listening on port "
    << serverPort.get_future().get();

  serverThread.join();

  return 0;
}
