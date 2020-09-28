/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <asio/ssl.hpp>

#include "crystal/foundation/http/ClientBase.h"

namespace crystal {

using HTTPS = asio::ssl::stream<asio::ip::tcp::socket>;

class HttpsClient : public ClientBase<HTTPS> {
 public:
  /**
   * Constructs a client object.
   *
   * @param server_port_path   Server resource given by host[:port][/path]
   * @param verify_certificate Set to true (default) to verify the server's
   *                           certificate and hostname according to RFC 2818.
   * @param certification_file If non-empty, sends the given certification file
   *                           to server. Requires private_key_file.
   * @param private_key_file   If non-empty, specifies the file containing the
   *                           private key for certification_file.
   *                           Requires certification_file.
   * @param verify_file        If non-empty, use this certificate authority file
   *                           to perform verification.
   */
  HttpsClient(const std::string& server_port_path,
              bool verify_certificate = true,
              const std::string& certification_file = std::string(),
              const std::string& private_key_file = std::string(),
              const std::string& verify_file = std::string());

 protected:
  std::shared_ptr<Connection<HTTPS>> create_connection() override;

  void connect(const std::shared_ptr<Session>& session) override;

  void handshake(const std::shared_ptr<Session>& session);

  asio::ssl::context context_;
};

} // namespace crystal
