/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/HttpServer.h"

namespace crystal {

void HttpServer::accept() {
  auto connection = create_connection(*io_service);

  acceptor_->async_accept(
      *connection->socket,
      [this, connection](const std::error_code& ec) {
        auto lock = connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }

        // Immediately start accepting a new connection (unless io_service has
        // been stopped)
        if (ec != asio::error::operation_aborted) {
          this->accept();
        }

        auto session = std::make_shared<Session>(
            config.max_request_streambuf_size, connection);

        if (!ec) {
          asio::ip::tcp::no_delay option(true);
          std::error_code ec;
          session->connection->socket->set_option(option, ec);

          this->read(session);
        } else if (this->on_error) {
          this->on_error(session->request, ec);
        }
      });
}

}  // namespace crystal
