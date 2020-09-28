/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <mutex>
#include <unordered_set>

#include "crystal/foundation/http/ClientConfig.h"
#include "crystal/foundation/http/ClientResponse.h"
#include "crystal/foundation/http/ClientSession.h"
#include "crystal/foundation/http/HttpHeader.h"
#include "crystal/foundation/http/detail/Message.h"

namespace crystal {

template <class SocketT>
class ClientBase {
 public:
  using Response = ClientResponse<SocketT>;
  using Session = ClientSession<SocketT>;

  /// Set before calling a request function.
  ClientConfig config;

  /// If you want to reuse an already created asio::io_service, store its
  /// pointer here before calling a request function. Do not set when using
  /// synchronous request functions.
  std::shared_ptr<asio::io_context> io_service;

  /// Convenience function to perform synchronous request. The io_service is
  /// started in this function. Should not be combined with asynchronous request
  /// functions. If you reuse the io_service for other tasks, use the
  /// asynchronous request functions instead. When requesting Server-Sent
  /// Events: will throw on error::eof, please use asynchronous request
  /// functions instead.
  std::shared_ptr<Response> request(
      const std::string& method,
      const std::string& path = {"/"},
      std::string_view content = {},
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap()) {
    return sync_request(method, path, content, header);
  }

  /// Convenience function to perform synchronous request. The io_service is
  /// started in this function. Should not be combined with asynchronous request
  /// functions. If you reuse the io_service for other tasks, use the
  /// asynchronous request functions instead. When requesting Server-Sent
  /// Events: will throw on error::eof, please use asynchronous request
  /// functions instead.
  std::shared_ptr<Response> request(
      const std::string& method,
      const std::string& path,
      std::istream& content,
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap()) {
    return sync_request(method, path, content, header);
  }

  /// Asynchronous request where running Client's io_service is required.
  /// Do not use concurrently with the synchronous request functions.
  /// When requesting Server-Sent Events: request_callback might be called more
  /// than twice, first call with empty contents on open, and with ec =
  /// error::eof on last call
  void request(
      const std::string& method,
      const std::string& path,
      std::string_view content,
      const CaseInsensitiveMultimap& header,
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>&& request_callback_);

  /// Asynchronous request where running Client's io_service is required.
  /// Do not use concurrently with the synchronous request functions.
  /// When requesting Server-Sent Events: request_callback might be called more
  /// than twice, first call with empty contents on open, and with ec =
  /// error::eof on last call
  void request(
      const std::string& method,
      const std::string& path,
      std::string_view content,
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>&& request_callback_) {
    request(method, path, content, CaseInsensitiveMultimap(),
            std::move(request_callback_));
  }

  /// Asynchronous request where running Client's io_service is required.
  /// Do not use concurrently with the synchronous request functions.
  /// When requesting Server-Sent Events: request_callback might be called more
  /// than twice, first call with empty contents on open, and with ec =
  /// error::eof on last call
  void request(
      const std::string& method,
      const std::string& path,
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>&& request_callback_) {
    request(method, path, std::string(), CaseInsensitiveMultimap(),
            std::move(request_callback_));
  }

  /// Asynchronous request where running Client's io_service is required.
  /// Do not use concurrently with the synchronous request functions.
  /// When requesting Server-Sent Events: request_callback might be called more
  /// than twice, first call with empty contents on open, and with ec =
  /// error::eof on last call
  void request(
      const std::string& method,
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>&& request_callback_) {
    request(method, std::string("/"), std::string(), CaseInsensitiveMultimap(),
            std::move(request_callback_));
  }

  /// Asynchronous request where running Client's io_service is required.
  /// Do not use concurrently with the synchronous request functions.
  /// When requesting Server-Sent Events: request_callback might be called more
  /// than twice, first call with empty contents on open, and with ec =
  /// error::eof on last call
  void request(
      const std::string& method,
      const std::string& path,
      std::istream& content,
      const CaseInsensitiveMultimap& header,
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>&& request_callback_);

  /// Asynchronous request where running Client's io_service is required.
  /// Do not use concurrently with the synchronous request functions.
  /// When requesting Server-Sent Events: request_callback might be called more
  /// than twice, first call with empty contents on open, and with ec =
  /// error::eof on last call
  void request(
      const std::string& method,
      const std::string& path,
      std::istream& content,
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>&& request_callback_) {
    request(method, path, content, CaseInsensitiveMultimap(),
            std::move(request_callback_));
  }

  /// Close connections.
  void stop();

  virtual ~ClientBase();

 protected:
  ClientBase(const std::string& host_port, unsigned short default_port);

  template <typename ContentType>
  std::shared_ptr<Response> sync_request(
      const std::string& method,
      const std::string& path,
      ContentType& content,
      const CaseInsensitiveMultimap& header);

  std::shared_ptr<Connection<SocketT>> get_connection();

  std::pair<std::string, unsigned short> parse_host_port(
      const std::string& host_port, unsigned short default_port) const;

  virtual std::shared_ptr<Connection<SocketT>> create_connection() = 0;
  virtual void connect(const std::shared_ptr<Session>&) = 0;

  std::unique_ptr<asio::streambuf> create_request_header(
      const std::string& method,
      const std::string& path,
      const CaseInsensitiveMultimap& header) const;

  void write(const std::shared_ptr<Session>& session);

  void read(const std::shared_ptr<Session>& session);

  void reconnect(
      const std::shared_ptr<Session>& session, const std::error_code& ec);

  void read_content(
      const std::shared_ptr<Session>& session, size_t remaining_length);

  void read_content(const std::shared_ptr<Session>& session);

  void read_chunked_transfer_encoded(
      const std::shared_ptr<Session>& session,
      const std::shared_ptr<asio::streambuf>& chunk_size_streambuf);

  void read_server_sent_event(
      const std::shared_ptr<Session>& session,
      const std::shared_ptr<asio::streambuf>& events_streambuf);

  bool internal_io_service_ = false;

  std::string host_;
  unsigned short port_;
  unsigned short default_port_;

  std::unique_ptr<std::pair<std::string, std::string>> host_port_;

  std::mutex connections_mutex_;
  std::unordered_set<std::shared_ptr<Connection<SocketT>>> connections_;

  std::shared_ptr<ScopeRunner> handler_runner_;

  std::mutex synchronous_request_mutex_;
  bool synchronous_request_called_ = false;
};

//////////////////////////////////////////////////////////////////////

template <class SocketT>
ClientBase<SocketT>::ClientBase(
    const std::string& host_port, unsigned short default_port)
    : default_port_(default_port), handler_runner_(new ScopeRunner()) {
  auto parsed_host_port = parse_host_port(host_port, default_port);
  host_ = parsed_host_port.first;
  port_ = parsed_host_port.second;
}

template <class SocketT>
ClientBase<SocketT>::~ClientBase() {
  handler_runner_->stop();
  stop();
  if (internal_io_service_) {
    io_service->stop();
  }
}

template <class SocketT>
void ClientBase<SocketT>::request(
    const std::string& method,
    const std::string& path,
    std::string_view content,
    const CaseInsensitiveMultimap& header,
    std::function<void(std::shared_ptr<Response>,
                       const std::error_code&)>&& request_callback_) {
  auto session = std::make_shared<Session>(
      config.max_response_streambuf_size,
      get_connection(),
      create_request_header(method, path, header));
  // To avoid keeping session alive longer than needed
  std::weak_ptr<Session> session_weak(session);
  auto request_callback = std::make_shared<
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>>(
        std::move(request_callback_));
  session->callback =
    [this, session_weak, request_callback](const std::error_code& ec) {
      if (auto session = session_weak.lock()) {
        if (session->response->content.end) {
          session->connection->cancel_timeout();
          session->connection->in_use = false;
        }
        {
          std::lock_guard lock(this->connections_mutex_);

          // Remove unused connections, but keep one open for HTTP persistent
          // connection:
          size_t unused_connections = 0;
          for (auto it = this->connections_.begin();
               it != this->connections_.end();) {
            if (ec && session->connection == *it) {
              it = this->connections_.erase(it);
            } else if ((*it)->in_use) {
              ++it;
            } else {
              ++unused_connections;
              if (unused_connections > 1) {
                it = this->connections_.erase(it);
              } else {
                ++it;
              }
            }
          }
        }

        if (*request_callback) {
          (*request_callback)(session->response, ec);
        }
      }
    };

  std::ostream write_stream(session->request_streambuf.get());
  if (content.size() > 0) {
    auto header_it = header.find("Content-Length");
    if (header_it == header.end()) {
      header_it = header.find("Transfer-Encoding");
      if (header_it == header.end() || header_it->second != "chunked") {
        write_stream << "Content-Length: " << content.size() << "\r\n";
      }
    }
  }
  write_stream << "\r\n";
  write_stream.write(content.data(),
                     static_cast<std::streamsize>(content.size()));

  connect(session);
}

template <class SocketT>
void ClientBase<SocketT>::request(
    const std::string& method,
    const std::string& path,
    std::istream& content,
    const CaseInsensitiveMultimap& header,
    std::function<void(std::shared_ptr<Response>,
                       const std::error_code&)>&& request_callback_) {
  auto session = std::make_shared<Session>(
      config.max_response_streambuf_size,
      get_connection(),
      create_request_header(method, path, header));
  // To avoid keeping session alive longer than needed
  std::weak_ptr<Session> session_weak(session);
  auto request_callback = std::make_shared<
      std::function<void(std::shared_ptr<Response>,
                         const std::error_code&)>>(
        std::move(request_callback_));
  session->callback =
    [this, session_weak, request_callback](const std::error_code& ec) {
      if (auto session = session_weak.lock()) {
        if (session->response->content.end) {
          session->connection->cancel_timeout();
          session->connection->in_use = false;
        }
        {
          std::lock_guard lock(this->connections_mutex_);

          // Remove unused connections, but keep one open for HTTP persistent
          // connection:
          size_t unused_connections = 0;
          for (auto it = this->connections_.begin();
               it != this->connections_.end();) {
            if (ec && session->connection == *it) {
              it = this->connections_.erase(it);
            } else if ((*it)->in_use) {
              ++it;
            } else {
              ++unused_connections;
              if (unused_connections > 1) {
                it = this->connections_.erase(it);
              } else {
                ++it;
              }
            }
          }
        }

        if (*request_callback) {
          (*request_callback)(session->response, ec);
        }
      }
    };

  content.seekg(0, std::ios::end);
  auto content_length = content.tellg();
  content.seekg(0, std::ios::beg);
  std::ostream write_stream(session->request_streambuf.get());
  if (content_length > 0) {
    auto header_it = header.find("Content-Length");
    if (header_it == header.end()) {
      header_it = header.find("Transfer-Encoding");
      if (header_it == header.end() || header_it->second != "chunked") {
        write_stream << "Content-Length: " << content_length << "\r\n";
      }
    }
  }
  write_stream << "\r\n";
  if (content_length > 0) {
    write_stream << content.rdbuf();
  }

  connect(session);
}

template <class SocketT>
void ClientBase<SocketT>::stop() {
  std::lock_guard lock(connections_mutex_);
  for (auto it = connections_.begin(); it != connections_.end();) {
    (*it)->close();
    it = connections_.erase(it);
  }
}

template <class SocketT>
template <typename ContentType>
std::shared_ptr<typename ClientBase<SocketT>::Response>
ClientBase<SocketT>::sync_request(
    const std::string& method,
    const std::string& path,
    ContentType& content,
    const CaseInsensitiveMultimap& header) {
  {
    std::lock_guard lock(synchronous_request_mutex_);
    if (!synchronous_request_called_) {
      if (io_service) { // Throw if io_service already set
        throw std::make_error_code(std::errc::operation_not_permitted);
      }
      io_service = std::make_shared<asio::io_context>();
      internal_io_service_ = true;
      auto io_service_ = io_service;
      std::thread thread([io_service_] {
        auto work = make_work_guard(*io_service_);
        io_service_->run();
      });
      thread.detach();
      synchronous_request_called_ = true;
    }
  }

  std::shared_ptr<Response> response;
  std::promise<std::shared_ptr<Response>> response_promise;
  auto stop_future_handlers = std::make_shared<bool>(false);
  request(
      method, path, content, header,
      [&response, &response_promise, stop_future_handlers](
          std::shared_ptr<Response> response_,
          std::error_code ec) {
        if (*stop_future_handlers) {
          return;
        }

        if (!response) {
          response = response_;
        } else if (!ec) {
          if (response_->streambuf.size() + response->streambuf.size() >
              response->streambuf.max_size()) {
            ec = std::make_error_code(std::errc::message_size);
            response->close();
          } else {
            // Move partial response_ content to response:
            auto& source = response_->streambuf;
            auto& target = response->streambuf;
            target.commit(asio::buffer_copy(target.prepare(source.size()),
                                            source.data()));
            source.consume(source.size());
          }
        }

        if (ec) {
          response_promise.set_exception(
              std::make_exception_ptr(std::system_error(ec)));
          *stop_future_handlers = true;
        } else if (response_->content.end) {
          response_promise.set_value(response);
        }
      });

  return response_promise.get_future().get();
}

template <class SocketT>
std::shared_ptr<Connection<SocketT>> ClientBase<SocketT>::get_connection() {
  std::shared_ptr<Connection<SocketT>> connection;
  std::lock_guard lock(connections_mutex_);

  if (!io_service) {
    io_service = std::make_shared<asio::io_context>();
    internal_io_service_ = true;
  }

  for (auto it = connections_.begin(); it != connections_.end(); ++it) {
    if (!(*it)->in_use) {
      connection = *it;
      break;
    }
  }
  if (!connection) {
    connection = create_connection();
    connections_.emplace(connection);
  }
  connection->attempt_reconnect = true;
  connection->in_use = true;

  if (!host_port_) {
    if (config.proxy_server.empty()) {
      host_port_ = std::make_unique<std::pair<std::string, std::string>>(
          host_, std::to_string(port_));
    } else {
      auto proxy_host_port = parse_host_port(config.proxy_server, 8080);
      host_port_ = std::make_unique<std::pair<std::string, std::string>>(
          proxy_host_port.first, std::to_string(proxy_host_port.second));
    }
  }

  return connection;
}

template <class SocketT>
std::pair<std::string, unsigned short> ClientBase<SocketT>::parse_host_port(
    const std::string& host_port, unsigned short default_port) const {
  std::pair<std::string, unsigned short> parsed_host_port;
  size_t host_end = host_port.find(':');
  if (host_end == std::string::npos) {
    parsed_host_port.first = host_port;
    parsed_host_port.second = default_port;
  } else {
    parsed_host_port.first = host_port.substr(0, host_end);
    try {
      parsed_host_port.second = static_cast<unsigned short>(
          std::stoul(host_port.substr(host_end + 1)));
    } catch (...) {
      parsed_host_port.second = default_port;
    }
  }
  return parsed_host_port;
}

template <class SocketT>
std::unique_ptr<asio::streambuf> ClientBase<SocketT>::create_request_header(
    const std::string& method,
    const std::string& path,
    const CaseInsensitiveMultimap& header) const {
  auto corrected_path = path;
  if (corrected_path == "") {
    corrected_path = "/";
  }
  if (!config.proxy_server.empty() &&
      std::is_same<SocketT, asio::ip::tcp::socket>::value) {
    corrected_path =
        "http://" + host_ + ':' + std::to_string(port_) + corrected_path;
  }

  std::unique_ptr<asio::streambuf> streambuf(new asio::streambuf());
  std::ostream write_stream(streambuf.get());
  write_stream << method << " " << corrected_path << " HTTP/1.1\r\n";
  write_stream << "Host: " << host_;
  if (port_ != default_port_) {
    write_stream << ':' << std::to_string(port_);
  }
  write_stream << "\r\n";
  for (auto& h : header) {
    write_stream << h.first << ": " << h.second << "\r\n";
  }
  return streambuf;
}

template <class SocketT>
void ClientBase<SocketT>::write(
    const std::shared_ptr<Session>& session) {
  session->connection->set_timeout(config.timeout);
  asio::async_write(
      *session->connection->socket, session->request_streambuf->data(),
      [this, session](const std::error_code& ec, size_t /*bytes_transferred*/) {
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }
        if (!ec) {
          this->read(session);
        } else {
          if (session->connection->attempt_reconnect &&
              ec != asio::error::operation_aborted) {
            reconnect(session, ec);
          } else {
            session->callback(ec);
          }
        }
      });
}

template <class SocketT>
void ClientBase<SocketT>::read(
    const std::shared_ptr<Session>& session) {
  asio::async_read_until(
      *session->connection->socket, session->response->streambuf,
      HeaderEndMatch(),
      [this, session](const std::error_code& ec, size_t bytes_transferred) {
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) return;

        if (!ec) {
          session->connection->attempt_reconnect = true;
          size_t num_additional_bytes =
              session->response->streambuf.size() - bytes_transferred;

          if (!ResponseMessage::parse(session->response->content,
                                      session->response->http_version,
                                      session->response->status_code,
                                      session->response->header)) {
            session->callback(std::make_error_code(std::errc::protocol_error));
            return;
          }

          auto header_it = session->response->header.find("Content-Length");
          if (header_it != session->response->header.end()) {
            auto content_length = std::stoull(header_it->second);
            if (content_length > num_additional_bytes) {
              this->read_content(session,
                                 content_length - num_additional_bytes);
            }
            else {
              session->callback(ec);
            }
          } else if ((header_it =
                      session->response->header.find("Transfer-Encoding")) !=
                         session->response->header.end() &&
                     header_it->second == "chunked") {
            // Expect hex number to not exceed 16 bytes (64-bit number), but
            // take into account previous additional read bytes
            auto chunk_size_streambuf =
                std::make_shared<asio::streambuf>(std::max<size_t>(
                    16 + 2, session->response->streambuf.size()));

            // Move leftover bytes
            auto& source = session->response->streambuf;
            auto& target = *chunk_size_streambuf;
            target.commit(asio::buffer_copy(target.prepare(source.size()),
                                            source.data()));
            source.consume(source.size());

            this->read_chunked_transfer_encoded(session, chunk_size_streambuf);
          } else if (session->response->http_version < "1.1" ||
                     ((header_it =
                       session->response->header.find("Connection")) !=
                          session->response->header.end() &&
                      header_it->second == "close")) {
            read_content(session);
          } else if ((header_it =
                      session->response->header.find("Content-Type")) !=
                         session->response->header.end() &&
                     header_it->second == "text/event-stream") {
            auto events_streambuf = std::make_shared<asio::streambuf>(
                this->config.max_response_streambuf_size);

            // Move leftover bytes
            auto& source = session->response->streambuf;
            auto& target = *events_streambuf;
            target.commit(asio::buffer_copy(target.prepare(source.size()),
                                            source.data()));
            source.consume(source.size());

            // Connection to a Server-Sent Events resource is opened
            session->callback(ec);

            this->read_server_sent_event(session, events_streambuf);
          } else {
            session->callback(ec);
          }
        } else {
          if (session->connection->attempt_reconnect &&
              ec != asio::error::operation_aborted) {
            reconnect(session, ec);
          } else {
            session->callback(ec);
          }
        }
      });
}

template <class SocketT>
void ClientBase<SocketT>::reconnect(
    const std::shared_ptr<Session>& session, const std::error_code& ec) {
  std::unique_lock<std::mutex> lock(connections_mutex_);
  auto it = connections_.find(session->connection);
  if (it != connections_.end()) {
    connections_.erase(it);
    session->connection = create_connection();
    session->connection->attempt_reconnect = false;
    session->connection->in_use = true;
    session->response = std::make_shared<Response>(
        this->config.max_response_streambuf_size, session->connection);
    connections_.emplace(session->connection);
    lock.unlock();
    this->connect(session);
  } else {
    lock.unlock();
    session->callback(ec);
  }
}

template <class SocketT>
void ClientBase<SocketT>::read_content(
    const std::shared_ptr<Session>& session, size_t remaining_length) {
  asio::async_read(
      *session->connection->socket, session->response->streambuf,
      asio::transfer_exactly(remaining_length),
      [this, session, remaining_length](const std::error_code& ec,
                                        size_t bytes_transferred) {
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }

        if (!ec) {
          if (session->response->streambuf.size() ==
                  session->response->streambuf.max_size() &&
              remaining_length > bytes_transferred) {
            session->response->content.end = false;
            session->callback(ec);
            session->response = std::make_shared<Response>(*session->response);
            this->read_content(session, remaining_length - bytes_transferred);
          } else {
            session->callback(ec);
          }
        } else {
          session->callback(ec);
        }
      });
}

template <class SocketT>
void ClientBase<SocketT>::read_content(
    const std::shared_ptr<Session>& session) {
  asio::async_read(
      *session->connection->socket, session->response->streambuf,
      [this, session](const std::error_code& ec_,
                      size_t /*bytes_transferred*/) {
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }

        auto ec = ec_ == asio::error::eof ? std::error_code() : ec_;

        if (!ec) {
          {
            std::lock_guard lock(this->connections_mutex_);
            this->connections_.erase(session->connection);
          }
          if (session->response->streambuf.size() ==
              session->response->streambuf.max_size()) {
            session->response->content.end = false;
            session->callback(ec);
            session->response = std::make_shared<Response>(*session->response);
            this->read_content(session);
          } else {
            session->callback(ec);
          }
        } else {
          session->callback(ec);
        }
      });
}

template <class SocketT>
void ClientBase<SocketT>::read_chunked_transfer_encoded(
    const std::shared_ptr<Session>& session,
    const std::shared_ptr<asio::streambuf>& chunk_size_streambuf) {
  asio::async_read_until(
      *session->connection->socket, *chunk_size_streambuf, "\r\n",
      [this, session, chunk_size_streambuf](const std::error_code& ec,
                                            size_t bytes_transferred) {
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }

        if (!ec) {
          std::istream istream(chunk_size_streambuf.get());
          std::string line;
          std::getline(istream, line);
          bytes_transferred -= line.size() + 1;
          unsigned long chunk_size = 0;
          try {
            chunk_size = std::stoul(line, 0, 16);
          } catch (...) {
            session->callback(std::make_error_code(std::errc::protocol_error));
            return;
          }

          if (chunk_size == 0) {
            session->callback(std::error_code());
            return;
          }

          if (chunk_size + session->response->streambuf.size() >
              session->response->streambuf.max_size()) {
            session->response->content.end = false;
            session->callback(ec);
            session->response = std::make_shared<Response>(*session->response);
          }

          auto num_additional_bytes =
              chunk_size_streambuf->size() - bytes_transferred;

          auto bytes_to_move =
              std::min<size_t>(chunk_size, num_additional_bytes);
          if (bytes_to_move > 0) {
            auto& source = *chunk_size_streambuf;
            auto& target = session->response->streambuf;
            target.commit(asio::buffer_copy(target.prepare(bytes_to_move),
                                            source.data(), bytes_to_move));
            source.consume(bytes_to_move);
          }

          if (chunk_size > num_additional_bytes) {
            asio::async_read(
                *session->connection->socket, session->response->streambuf,
                asio::transfer_exactly(chunk_size - num_additional_bytes),
                [this, session, chunk_size_streambuf](
                    const std::error_code& ec, size_t /*bytes_transferred*/) {
                  auto lock =
                      session->connection->handler_runner->continue_lock();
                  if (!lock) {
                    return;
                  }

                  if (!ec) {
                    // Remove "\r\n"
                    auto null_buffer = std::make_shared<asio::streambuf>(2);
                    asio::async_read(
                        *session->connection->socket,
                        *null_buffer,
                        asio::transfer_exactly(2),
                        [this, session, chunk_size_streambuf, null_buffer](
                            const std::error_code& ec,
                            size_t /*bytes_transferred*/) {
                          auto lock = session->connection->handler_runner
                                          ->continue_lock();
                          if (!lock) {
                            return;
                          }
                          if (!ec) {
                            read_chunked_transfer_encoded(
                                session, chunk_size_streambuf);
                          } else {
                            session->callback(ec);
                          }
                        });
                  } else {
                    session->callback(ec);
                  }
                });
          } else if (2 + chunk_size >
                     num_additional_bytes) {  // If only end of chunk remains
                                              // unread (\n or \r\n)
            // Remove "\r\n"
            if (2 + chunk_size - num_additional_bytes == 1) {
              istream.get();
            }
            auto null_buffer = std::make_shared<asio::streambuf>(2);
            asio::async_read(
                *session->connection->socket,
                *null_buffer,
                asio::transfer_exactly(2 + chunk_size - num_additional_bytes),
                [this, session, chunk_size_streambuf, null_buffer](
                    const std::error_code& ec, size_t /*bytes_transferred*/) {
                  auto lock =
                      session->connection->handler_runner->continue_lock();
                  if (!lock) {
                    return;
                  }
                  if (!ec) {
                    read_chunked_transfer_encoded(session,
                                                  chunk_size_streambuf);
                  } else {
                    session->callback(ec);
                  }
                });
          } else {
            // Remove "\r\n"
            istream.get();
            istream.get();

            read_chunked_transfer_encoded(session, chunk_size_streambuf);
          }
        } else {
          session->callback(ec);
        }
      });
}

template <class SocketT>
void ClientBase<SocketT>::read_server_sent_event(
    const std::shared_ptr<Session>& session,
    const std::shared_ptr<asio::streambuf>& events_streambuf) {
  asio::async_read_until(
      *session->connection->socket, *events_streambuf, HeaderEndMatch(),
      [this, session, events_streambuf](const std::error_code& ec,
                                        size_t /*bytes_transferred*/) {
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }

        if (!ec) {
          session->response->content.end = false;
          std::istream istream(events_streambuf.get());
          std::ostream ostream(&session->response->streambuf);
          std::string line;
          while (std::getline(istream, line) &&
                 !line.empty() &&
                 !(line.back() == '\r' && line.size() == 1)) {
            ostream.write(line.data(),
                          static_cast<std::streamsize>(
                              line.size() - (line.back() == '\r' ? 1 : 0)));
            ostream.put('\n');
          }

          session->callback(ec);
          session->response = std::make_shared<Response>(*session->response);
          read_server_sent_event(session, events_streambuf);
        } else {
          session->callback(ec);
        }
      });
}

}  // namespace crystal
