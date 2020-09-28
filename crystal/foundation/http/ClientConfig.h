/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <limits>
#include <string>

namespace crystal {

struct ClientConfig {
  /// Set timeout on requests in seconds. Default value: 0 (no timeout).
  long timeout = 0;
  /// Set connect timeout in seconds. Default value: 0 (Config::timeout is
  /// then used instead).
  long timeout_connect = 0;
  /// Maximum size of response stream buffer. Defaults to architecture
  /// maximum. Reaching this limit will result in a message_size error code.
  size_t max_response_streambuf_size = std::numeric_limits<size_t>::max();
  /// Set proxy server (server:port)
  std::string proxy_server;
};

}  // namespace crystal
