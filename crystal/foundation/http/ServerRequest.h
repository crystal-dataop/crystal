/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <regex>
#include <asio.hpp>

#include "crystal/foundation/http/Utility.h"
#include "crystal/foundation/http/detail/Connection.h"
#include "crystal/foundation/http/detail/Content.h"

namespace crystal {

template <class SocketT>
class ServerRequest {
 public:
  ServerRequest(size_t max_request_streambuf_size,
          const std::shared_ptr<Connection<SocketT>>& connection)
      : streambuf(max_request_streambuf_size),
        connection_(connection),
        content(streambuf) {}

  asio::ip::tcp::endpoint remote_endpoint() const;
  asio::ip::tcp::endpoint local_endpoint() const;

  /// Returns query keys with percent-decoded values.
  CaseInsensitiveMultimap parse_query_string() const;

 public:
  asio::streambuf streambuf;
  Content content;

  std::string method, path, query_string, http_version;

  CaseInsensitiveMultimap header;

  /// The result of the resource regular expression match of the request path.
  std::smatch path_match;

  /// The time point when the request header was fully read.
  std::chrono::system_clock::time_point header_read_time;

 private:
  std::weak_ptr<Connection<SocketT>> connection_;
};

//////////////////////////////////////////////////////////////////////

template <class SocketT>
inline asio::ip::tcp::endpoint ServerRequest<SocketT>::remote_endpoint() const {
  try {
    if (auto connection = connection_.lock())
      return connection->socket->lowest_layer().remote_endpoint();
  } catch (...) {
  }
  return asio::ip::tcp::endpoint();
}

template <class SocketT>
inline asio::ip::tcp::endpoint ServerRequest<SocketT>::local_endpoint() const {
  try {
    if (auto connection = connection_.lock())
      return connection->socket->lowest_layer().local_endpoint();
  } catch (...) {
  }
  return asio::ip::tcp::endpoint();
}

template <class SocketT>
inline CaseInsensitiveMultimap
ServerRequest<SocketT>::parse_query_string() const {
  return QueryString::parse(query_string);
}

}  // namespace crystal
