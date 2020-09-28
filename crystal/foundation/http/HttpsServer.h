/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <asio/ssl.hpp>

#include "crystal/foundation/http/ServerBase.h"

namespace crystal {

using HTTPS = asio::ssl::stream<asio::ip::tcp::socket>;

class HttpsServer : public ServerBase<HTTPS> {
 public:
  /**
   * Constructs a server object.
   *
   * @param certification_file If non-empty, sends the given certification file
   *                           to client.
   * @param private_key_file   Specifies the file containing the private key for
   *                           certification_file.
   * @param verify_file        If non-empty, use this certificate authority file
   *                           to perform verification of client's certificate
   *                           and hostname according to RFC 2818.
   */
  HttpsServer(const std::string& certification_file,
              const std::string& private_key_file,
              const std::string& verify_file = std::string());

 protected:
  void after_bind() override;

  void accept() override;

  asio::ssl::context context_;
  bool set_session_id_context_ = false;
};

}  // namespace crystal
