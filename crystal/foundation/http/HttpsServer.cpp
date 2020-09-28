/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/HttpsServer.h"

namespace crystal {

HttpsServer::HttpsServer(const std::string& certification_file,
                         const std::string& private_key_file,
                         const std::string& verify_file)
    : ServerBase(443), context_(asio::ssl::context::tlsv12) {
  context_.use_certificate_chain_file(certification_file);
  context_.use_private_key_file(private_key_file, asio::ssl::context::pem);

  if (verify_file.size() > 0) {
    context_.load_verify_file(verify_file);
    context_.set_verify_mode(asio::ssl::verify_peer |
                             asio::ssl::verify_fail_if_no_peer_cert |
                             asio::ssl::verify_client_once);
    set_session_id_context_ = true;
  }
}

void HttpsServer::after_bind() {
  if (set_session_id_context_) {
    // Creating session_id_context from address:port but reversed due to small
    // SSL_MAX_SSL_SESSION_ID_LENGTH
    auto session_id_context =
        std::to_string(acceptor_->local_endpoint().port()) + ':';
    session_id_context.append(config.address.rbegin(), config.address.rend());
    SSL_CTX_set_session_id_context(
        context_.native_handle(),
        reinterpret_cast<const unsigned char*>(session_id_context.data()),
        static_cast<unsigned int>(std::min<size_t>(
            session_id_context.size(), SSL_MAX_SSL_SESSION_ID_LENGTH)));
  }
}

void HttpsServer::accept() {
  auto connection = create_connection(*io_service, context_);

  acceptor_->async_accept(
      connection->socket->lowest_layer(),
      [this, connection](const std::error_code& ec) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }

        if (ec != asio::error::operation_aborted) {
          this->accept();
        }

        auto session = std::make_shared<Session>(
            config.max_request_streambuf_size, connection);

        if (!ec) {
          asio::ip::tcp::no_delay option(true);
          std::error_code ec;
          session->connection->socket->lowest_layer().set_option(option, ec);

          session->connection->set_timeout(config.timeout_request);
          session->connection->socket->async_handshake(
              asio::ssl::stream_base::server,
              [this, session](const std::error_code& ec) {
                session->connection->cancel_timeout();
                auto lock =
                    session->connection->handler_runner->continue_lock();
                if (!lock) {
                  return;
                }
                if (!ec) {
                  this->read(session);
                } else if (this->on_error) {
                  this->on_error(session->request, ec);
                }
              });
        } else if (this->on_error) {
          this->on_error(session->request, ec);
        }
      });
}

} // namespace crystal
