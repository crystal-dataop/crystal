/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include "crystal/foundation/http/ClientResponse.h"
#include "crystal/foundation/http/detail/Connection.h"

namespace crystal {

template <class SocketT>
class ClientSession {
 public:
  ClientSession(size_t max_response_streambuf_size,
                std::shared_ptr<Connection<SocketT>> connection_,
                std::unique_ptr<asio::streambuf> request_streambuf_)
      : connection(std::move(connection_)),
        request_streambuf(std::move(request_streambuf_)),
        response(new ClientResponse<SocketT>(max_response_streambuf_size, connection)) {}

  std::shared_ptr<Connection<SocketT>> connection;
  std::unique_ptr<asio::streambuf> request_streambuf;
  std::shared_ptr<ClientResponse<SocketT>> response;
  std::function<void(const std::error_code&)> callback;
};

}  // namespace crystal
