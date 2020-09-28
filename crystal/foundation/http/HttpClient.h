/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <asio.hpp>

#include "crystal/foundation/http/ClientBase.h"

namespace crystal {

using HTTP = asio::ip::tcp::socket;

class HttpClient : public ClientBase<HTTP> {
 public:
  /**
   * Constructs a client object.
   *
   * @param server_port_path Server resource given by host[:port][/path]
   */
  HttpClient(const std::string& server_port_path)
      : ClientBase(server_port_path, 80) {}

 protected:
  std::shared_ptr<Connection<HTTP>> create_connection() override;

  void connect(const std::shared_ptr<Session>& session) override;
};

}  // namespace crystal
