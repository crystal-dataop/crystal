/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include "crystal/foundation/http/ServerRequest.h"
#include "crystal/foundation/http/detail/Connection.h"

namespace crystal {

template <class SocketT>
class ServerSession {
 public:
  ServerSession(size_t max_request_streambuf_size,
          std::shared_ptr<Connection<SocketT>> connection_)
      : connection(std::move(connection_)),
        request(new ServerRequest<SocketT>(max_request_streambuf_size,
                                           connection)) {}

  std::shared_ptr<Connection<SocketT>> connection;
  std::shared_ptr<ServerRequest<SocketT>> request;
};

}  // namespace crystal
