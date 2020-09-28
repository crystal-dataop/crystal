/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <asio.hpp>

#include "crystal/foundation/http/Utility.h"
#include "crystal/foundation/http/detail/Connection.h"
#include "crystal/foundation/http/detail/Content.h"

namespace crystal {

template <class SocketT>
class ClientResponse {
 public:
  ClientResponse(size_t max_response_streambuf_size,
                 const std::shared_ptr<Connection<SocketT>>& connection)
      : streambuf(max_response_streambuf_size),
        connection_(connection),
        content(streambuf),
        shared_(new Shared()),
        http_version(shared_->http_version),
        status_code(shared_->status_code),
        header(shared_->header) {}

  ClientResponse(const ClientResponse& response)
      : streambuf(response.streambuf.max_size()),
        connection_(response.connection_),
        content(streambuf),
        shared_(response.shared_),
        http_version(shared_->http_version),
        status_code(shared_->status_code),
        header(shared_->header) {}

  /// Closes the connection to the server, preventing further response content
  /// parts from server.
  void close() {
    if (auto connection = connection_.lock()) {
      connection->close();
    }
  }

 private:
  struct Shared {
    std::string http_version, status_code;
    CaseInsensitiveMultimap header;
  };

  std::weak_ptr<Connection<SocketT>> connection_;
  std::shared_ptr<Shared> shared_;

 public:
  asio::streambuf streambuf;
  Content content;

  std::string &http_version, &status_code;

  CaseInsensitiveMultimap& header;
};

}  // namespace crystal
