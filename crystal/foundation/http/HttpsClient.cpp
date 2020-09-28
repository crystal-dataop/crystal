/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/HttpsClient.h"

namespace crystal {

HttpsClient::HttpsClient(const std::string& server_port_path,
                         bool verify_certificate,
                         const std::string& certification_file,
                         const std::string& private_key_file,
                         const std::string& verify_file)
    : ClientBase(server_port_path, 443),
      context_(asio::ssl::context::tlsv12) {
  if (certification_file.size() > 0 && private_key_file.size() > 0) {
    context_.use_certificate_chain_file(certification_file);
    context_.use_private_key_file(private_key_file, asio::ssl::context::pem);
  }

  if (verify_certificate) {
    context_.set_verify_callback(asio::ssl::rfc2818_verification(host_));
  }

  if (verify_file.size() > 0) {
    context_.load_verify_file(verify_file);
  } else {
    context_.set_default_verify_paths();
  }

  if (verify_certificate) {
    context_.set_verify_mode(asio::ssl::verify_peer);
  } else {
    context_.set_verify_mode(asio::ssl::verify_none);
  }
}

std::shared_ptr<Connection<HTTPS>> HttpsClient::create_connection() {
  return std::make_shared<Connection<HTTPS>>(
      handler_runner_, *io_service, context_);
}

void HttpsClient::connect(const std::shared_ptr<Session>& session) {
  if (!session->connection->socket->lowest_layer().is_open()) {
    auto resolver = std::make_shared<asio::ip::tcp::resolver>(*io_service);
    resolver->async_resolve(
        host_port_->first,
        host_port_->second,
        [this, session, resolver](
            const std::error_code& ec,
            asio::ip::tcp::resolver::results_type results) {
          auto lock = session->connection->handler_runner->continue_lock();
          if (!lock) {
            return;
          }
          if (!ec) {
            session->connection->set_timeout(this->config.timeout_connect);
            asio::async_connect(
                session->connection->socket->lowest_layer(), results,
                [this, session, resolver](
                    const std::error_code& ec,
                    asio::ip::tcp::endpoint /*endpoint*/) {
                  session->connection->cancel_timeout();
                  auto lock =
                      session->connection->handler_runner->continue_lock();
                  if (!lock) {
                    return;
                  }
                  if (!ec) {
                    asio::ip::tcp::no_delay option(true);
                    std::error_code ec;
                    session->connection->socket->lowest_layer().set_option(
                        option, ec);

                    if (!this->config.proxy_server.empty()) {
                      auto write_buffer = std::make_shared<asio::streambuf>();
                      std::ostream write_stream(write_buffer.get());
                      auto host_port =
                          this->host_ + ':' + std::to_string(this->port_);
                      write_stream << "CONNECT " + host_port + " HTTP/1.1\r\n"
                                   << "Host: " << host_port << "\r\n\r\n";
                      session->connection->set_timeout(
                          this->config.timeout_connect);
                      asio::async_write(
                          session->connection->socket->next_layer(),
                          *write_buffer,
                          [this, session, write_buffer](
                              const std::error_code& ec,
                              size_t /*bytes_transferred*/) {
                            session->connection->cancel_timeout();
                            auto lock = session->connection->handler_runner
                                            ->continue_lock();
                            if (!lock) {
                              return;
                            }
                            if (!ec) {
                              std::shared_ptr<Response>
                                response(new Response(
                                  this->config.max_response_streambuf_size,
                                  session->connection));
                              session->connection->set_timeout(
                                  this->config.timeout_connect);
                              asio::async_read_until(
                                  session->connection->socket->next_layer(),
                                  response->streambuf, "\r\n\r\n",
                                  [this, session, response](
                                      const std::error_code& ec,
                                      size_t /*bytes_transferred*/) {
                                    session->connection->cancel_timeout();
                                    auto lock =
                                        session->connection->handler_runner
                                            ->continue_lock();
                                    if (!lock) return;
                                    if (response->streambuf.size() ==
                                        response->streambuf.max_size()) {
                                      session->callback(
                                          std::make_error_code(
                                              std::errc::message_size));
                                      return;
                                    }

                                    if (!ec) {
                                      if (!ResponseMessage::parse(
                                              response->content,
                                              response->http_version,
                                              response->status_code,
                                              response->header)) {
                                        session->callback(
                                            std::make_error_code(
                                                std::errc::protocol_error));
                                      } else {
                                        if (response->status_code.compare(
                                                0, 3, "200") != 0) {
                                          session->callback(
                                              std::make_error_code(
                                                  std::errc::
                                                       permission_denied));
                                        } else {
                                          this->handshake(session);
                                        }
                                      }
                                    } else {
                                      session->callback(ec);
                                    }
                                  });
                            } else {
                              session->callback(ec);
                            }
                          });
                    } else {
                      this->handshake(session);
                    }
                  } else {
                    session->callback(ec);
                  }
                });
          } else {
            session->callback(ec);
          }
        });
  } else {
    write(session);
  }
}

void HttpsClient::handshake(
    const std::shared_ptr<Session>& session) {
  SSL_set_tlsext_host_name(session->connection->socket->native_handle(),
                           this->host_.c_str());

  session->connection->set_timeout(this->config.timeout_connect);
  session->connection->socket->async_handshake(
      asio::ssl::stream_base::client,
      [this, session](const std::error_code& ec) {
        session->connection->cancel_timeout();
        auto lock = session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }
        if (!ec) {
          this->write(session);
        } else {
          session->callback(ec);
        }
      });
}

} // namespace crystal
