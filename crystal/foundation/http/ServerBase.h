/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <map>
#include <regex>
#include <system_error>
#include <unordered_set>

#include "crystal/foundation/http/ServerConfig.h"
#include "crystal/foundation/http/ServerRequest.h"
#include "crystal/foundation/http/ServerResponse.h"
#include "crystal/foundation/http/detail/Message.h"

namespace crystal {

template <class SocketT>
class ServerBase {
 public:
  using Request = ServerRequest<SocketT>;
  using Response = ServerResponse<SocketT>;
  using Session = ServerSession<SocketT>;

  /// Set before calling start().
  ServerConfig config;

 private:
  class regex_orderable : public std::regex {
   public:
    regex_orderable(const char* regex_cstr)
        : std::regex(regex_cstr), str_(regex_cstr) {}
    regex_orderable(std::string regex_str_)
        : std::regex(regex_str_), str_(std::move(regex_str_)) {}

    bool operator<(const regex_orderable& rhs) const {
      return str_ < rhs.str_;
    }

   private:
    std::string str_;
  };

 public:
  /// Use this container to add resources for specific request paths depending
  /// on the given regex and method. Warning: do not add or remove resources
  /// after start() is called
  std::map<regex_orderable,
           std::map<std::string,
                    std::function<void(std::shared_ptr<Response>,
                                       std::shared_ptr<Request>)>>>
      resource;

  /// If the request path does not match a resource regex, this function is
  /// called.
  std::map<std::string,
           std::function<void(std::shared_ptr<Response>,
                              std::shared_ptr<Request>)>>
      default_resource;

  /// Called when an error occurs.
  std::function<void(std::shared_ptr<Request>,
                     const std::error_code&)>
      on_error;

  /// Called on upgrade requests.
  std::function<void(std::unique_ptr<SocketT>&,
                     std::shared_ptr<Request>)>
      on_upgrade;

  /// If you want to reuse an already created asio::io_service, store its
  /// pointer here before calling start().
  std::shared_ptr<asio::io_context> io_service;

  /// Start the server.
  /// If io_service is not set, an internal io_service is created instead.
  /// The callback argument is called after the server is accepting connections,
  /// where its parameter contains the assigned port.
  void start(
      const std::function<void(unsigned short /*port*/)>& callback = nullptr);

  /// Stop accepting new requests, and close current connections.
  void stop();

  virtual ~ServerBase() {
    handler_runner_->stop();
    stop();
  }

 protected:
  ServerBase(unsigned short port)
      : config(port),
        connections_(new Connections()),
        handler_runner_(new ScopeRunner()) {}

  virtual void after_bind() {}
  virtual void accept() = 0;

  template <typename... Args>
  std::shared_ptr<Connection<SocketT>> create_connection(Args&&... args);

  void read(const std::shared_ptr<Session>& session);

  void read_chunked_transfer_encoded(
      const std::shared_ptr<Session>& session,
      const std::shared_ptr<asio::streambuf>& chunk_size_streambuf);

  void find_resource(const std::shared_ptr<Session>& session);

  void write(const std::shared_ptr<Session>& session,
             std::function<void(std::shared_ptr<Response>,
                                std::shared_ptr<Request>)>&
                 resource_function);

  std::mutex start_stop_mutex_;

  bool internal_io_service_ = false;

  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
  std::vector<std::thread> threads_;

  struct Connections {
    std::mutex mutex;
    std::unordered_set<Connection<SocketT>*> set;
  };
  std::shared_ptr<Connections> connections_;

  std::shared_ptr<ScopeRunner> handler_runner_;
};

//////////////////////////////////////////////////////////////////////

template <class SocketT>
void ServerBase<SocketT>::start(
    const std::function<void(unsigned short /*port*/)>& callback) {
  std::unique_lock<std::mutex> lock(start_stop_mutex_);

  asio::ip::tcp::endpoint endpoint;
  if (!config.address.empty()) {
    endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(config.address),
                                       config.port);
  } else {
    endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v6(), config.port);
  }

  if (!io_service) {
    io_service = std::make_shared<asio::io_context>();
    internal_io_service_ = true;
  }

  if (!acceptor_) {
    acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(*io_service);
  }
  try {
    acceptor_->open(endpoint.protocol());
  } catch (const std::system_error& error) {
    if (error.code() == asio::error::address_family_not_supported &&
        config.address.empty()) {
      endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config.port);
      acceptor_->open(endpoint.protocol());
    } else {
      throw;
    }
  }
  acceptor_->set_option(asio::socket_base::reuse_address(config.reuse_address));
  if (config.fast_open) {
#if defined(__linux__) && defined(TCP_FASTOPEN)
    // This seems to be the value that is used in other examples.
    const int qlen = 5;
    std::error_code ec;
    acceptor_->set_option(
        asio::detail::socket_option::integer<IPPROTO_TCP, TCP_FASTOPEN>(qlen),
        ec);
#endif  // End Linux
  }
  acceptor_->bind(endpoint);

  after_bind();

  auto port = acceptor_->local_endpoint().port();

  acceptor_->listen();
  accept();

  if (internal_io_service_ && io_service->stopped()) {
    io_service->restart();
  }

  if (callback) {
    asio::post(*io_service, [callback, port] { callback(port); });
  }

  if (internal_io_service_) {
    // If thread_pool_size>1, start m_io_service.run() in (thread_pool_size-1)
    // threads for thread-pooling
    threads_.clear();
    for (size_t c = 1; c < config.thread_pool_size; c++) {
      threads_.emplace_back([this]() { this->io_service->run(); });
    }

    lock.unlock();

    // Main thread
    if (config.thread_pool_size > 0) {
      io_service->run();
    }

    lock.lock();

    // Wait for the rest of the threads, if any, to finish as well
    for (auto& t : threads_) {
      t.join();
    }
  }
}

template <class SocketT>
void ServerBase<SocketT>::stop() {
  std::lock_guard<std::mutex> lock(start_stop_mutex_);

  if (acceptor_) {
    std::error_code ec;
    acceptor_->close(ec);

    {
      std::lock_guard lock(connections_->mutex);
      for (auto& connection : connections_->set) {
        connection->close();
      }
      connections_->set.clear();
    }

    if (internal_io_service_) {
      io_service->stop();
    }
  }
}

template <class SocketT>
template <typename... Args>
std::shared_ptr<Connection<SocketT>>
ServerBase<SocketT>::create_connection(Args&&... args) {
  auto connections = this->connections_;
  auto connection = std::shared_ptr<Connection<SocketT>>(
      new Connection<SocketT>(handler_runner_, std::forward<Args>(args)...),
      [connections](Connection<SocketT>* connection) {
        {
          std::lock_guard lock(connections->mutex);
          auto it = connections->set.find(connection);
          if (it != connections->set.end()) {
            connections->set.erase(it);
          }
        }
        delete connection;
      });
  {
    std::lock_guard lock(connections->mutex);
    connections->set.emplace(connection.get());
  }
  return connection;
}

template <class SocketT>
void
ServerBase<SocketT>::read(const std::shared_ptr<Session>& session) {
  session->connection->set_timeout(config.timeout_request);
  asio::async_read_until(
      *session->connection->socket, session->request->streambuf, "\r\n\r\n",
      [this, session](const std::error_code& ec, size_t bytes_transferred) {
        session->connection->set_timeout(config.timeout_content);
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }
        session->request->header_read_time = std::chrono::system_clock::now();

        if (!ec) {
          // request->streambuf.size() is not necessarily the same as
          // bytes_transferred, from Boost-docs: "After a successful
          // async_read_until operation, the streambuf may contain additional
          // data beyond the delimiter" The chosen solution is to extract
          // lines from the stream directly when parsing the header. What is
          // left of the streambuf (maybe some bytes of the content) is
          // appended to in the async_read-function below (for retrieving
          // content).
          size_t num_additional_bytes =
              session->request->streambuf.size() - bytes_transferred;

          if (!RequestMessage::parse(session->request->content,
                                     session->request->method,
                                     session->request->path,
                                     session->request->query_string,
                                     session->request->http_version,
                                     session->request->header)) {
            if (this->on_error) {
              this->on_error(session->request,
                             std::make_error_code(std::errc::protocol_error));
            }
            return;
          }

          // If content, read that as well
          auto header_it = session->request->header.find("Content-Length");
          if (header_it != session->request->header.end()) {
            unsigned long long content_length = 0;
            try {
              content_length = std::stoull(header_it->second);
            } catch (const std::exception&) {
              if (this->on_error) {
                this->on_error(
                    session->request,
                    std::make_error_code(std::errc::protocol_error));
              }
              return;
            }
            if (content_length > session->request->streambuf.max_size()) {
              auto response = std::make_shared<Response>(
                  session, this->config.timeout_content);
              response->write(StatusCode::client_error_payload_too_large);
              if (this->on_error) {
                this->on_error(session->request,
                               std::make_error_code(std::errc::message_size));
              }
              return;
            }
            if (content_length > num_additional_bytes) {
              asio::async_read(
                  *session->connection->socket,
                  session->request->streambuf,
                  asio::transfer_exactly(content_length - num_additional_bytes),
                  [this, session](const std::error_code& ec,
                                  size_t /*bytes_transferred*/) {
                    auto lock =
                        session->connection->handler_runner->continue_lock();
                    if (!lock) {
                      return;
                    }

                    if (!ec) {
                      this->find_resource(session);
                    } else if (this->on_error) {
                      this->on_error(session->request, ec);
                    }
                  });
            } else {
              this->find_resource(session);
            }
          } else if ((header_it =
                      session->request->header.find("Transfer-Encoding")) !=
                         session->request->header.end() &&
                     header_it->second == "chunked") {
            // Expect hex number to not exceed 16 bytes (64-bit number), but
            // take into account previous additional read bytes
            auto chunk_size_streambuf =
                std::make_shared<asio::streambuf>(std::max<size_t>(
                    16 + 2, session->request->streambuf.size()));

            // Move leftover bytes
            auto& source = session->request->streambuf;
            auto& target = *chunk_size_streambuf;
            target.commit(asio::buffer_copy(target.prepare(source.size()),
                                            source.data()));
            source.consume(source.size());

            this->read_chunked_transfer_encoded(session,
                                                chunk_size_streambuf);
          } else {
            this->find_resource(session);
          }
        } else if (this->on_error) {
          this->on_error(session->request, ec);
        }
      });
}

template <class SocketT>
void ServerBase<SocketT>::read_chunked_transfer_encoded(
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
            if (this->on_error) {
              this->on_error(session->request,
                             std::make_error_code(std::errc::protocol_error));
            }
            return;
          }

          if (chunk_size == 0) {
            this->find_resource(session);
            return;
          }

          if (chunk_size + session->request->streambuf.size() >
              session->request->streambuf.max_size()) {
            auto response = std::make_shared<Response>(
                session, this->config.timeout_content);
            response->write(StatusCode::client_error_payload_too_large);
            if (this->on_error) {
              this->on_error(session->request,
                             std::make_error_code(std::errc::message_size));
            }
            return;
          }

          auto num_additional_bytes =
              chunk_size_streambuf->size() - bytes_transferred;

          auto bytes_to_move =
              std::min<size_t>(chunk_size, num_additional_bytes);
          if (bytes_to_move > 0) {
            // Move leftover bytes
            auto& source = *chunk_size_streambuf;
            auto& target = session->request->streambuf;
            target.commit(asio::buffer_copy(target.prepare(bytes_to_move),
                                            source.data(), bytes_to_move));
            source.consume(bytes_to_move);
          }

          if (chunk_size > num_additional_bytes) {
            asio::async_read(
                *session->connection->socket,
                session->request->streambuf,
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
                            this->on_error(session->request, ec);
                          }
                        });
                  } else if (this->on_error) {
                    this->on_error(session->request, ec);
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
                    this->on_error(session->request, ec);
                  }
                });
          } else {
            // Remove "\r\n"
            istream.get();
            istream.get();

            read_chunked_transfer_encoded(session, chunk_size_streambuf);
          }
        } else if (this->on_error) {
          this->on_error(session->request, ec);
        }
      });
}

template <class SocketT>
void ServerBase<SocketT>::find_resource(
    const std::shared_ptr<Session>& session) {
  // Upgrade connection
  if (on_upgrade) {
    auto it = session->request->header.find("Upgrade");
    if (it != session->request->header.end()) {
      // remove connection from connections
      {
        std::lock_guard lock(connections_->mutex);
        auto it = connections_->set.find(session->connection.get());
        if (it != connections_->set.end()) {
          connections_->set.erase(it);
        }
      }

      on_upgrade(session->connection->socket, session->request);
      return;
    }
  }
  // Find path- and method-match, and call write
  for (auto& regex_method : resource) {
    auto it = regex_method.second.find(session->request->method);
    if (it != regex_method.second.end()) {
      std::smatch sm_res;
      if (std::regex_match(session->request->path,
                           sm_res,
                           regex_method.first)) {
        session->request->path_match = std::move(sm_res);
        write(session, it->second);
        return;
      }
    }
  }
  auto it = default_resource.find(session->request->method);
  if (it != default_resource.end()) {
    write(session, it->second);
  }
}

template <class SocketT>
void ServerBase<SocketT>::write(
    const std::shared_ptr<Session>& session,
    std::function<
      void(std::shared_ptr<Response>,
           std::shared_ptr<Request>)>& resource_function) {
  auto response = std::shared_ptr<Response>(
      new ServerResponse<SocketT>(session, config.timeout_content),
      [this](Response* response_ptr) {
        auto response = std::shared_ptr<Response>(response_ptr);
        response->send_on_delete([this, response](const std::error_code& ec) {
          response->session->connection->cancel_timeout();
          if (!ec) {
            if (response->close_connection_after_response) {
              return;
            }

            auto range =
                response->session->request->header.equal_range("Connection");
            CaseInsensitiveEqual case_insensitive_equal;
            for (auto it = range.first; it != range.second; it++) {
              if (case_insensitive_equal(it->second, "close")) {
                return;
              } else if (case_insensitive_equal(it->second, "keep-alive")) {
                auto new_session = std::make_shared<Session>(
                    this->config.max_request_streambuf_size,
                    response->session->connection);
                this->read(new_session);
                return;
              }
            }
            if (response->session->request->http_version >= "1.1") {
              auto new_session = std::make_shared<Session>(
                  this->config.max_request_streambuf_size,
                  response->session->connection);
              this->read(new_session);
              return;
            }
          } else if (this->on_error) {
            this->on_error(response->session->request, ec);
          }
        });
      });

  try {
    resource_function(response, session->request);
  } catch (const std::exception&) {
    if (on_error) {
      on_error(session->request,
               std::make_error_code(std::errc::operation_canceled));
    }
    return;
  }
}

}  // namespace crystal
