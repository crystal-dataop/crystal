/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/HttpClient.h"

namespace crystal {

std::shared_ptr<Connection<HTTP>> HttpClient::create_connection() {
  return std::make_shared<Connection<HTTP>>(handler_runner_, *io_service);
}

void HttpClient::connect(const std::shared_ptr<Session>& session) {
  if (!session->connection->socket->lowest_layer().is_open()) {
    auto resolver = std::make_shared<asio::ip::tcp::resolver>(*io_service);
    session->connection->set_timeout(config.timeout_connect);
    resolver->async_resolve(
        host_port_->first,
        host_port_->second,
        [this, session, resolver](
            const std::error_code& ec,
            asio::ip::tcp::resolver::results_type results) {
          session->connection->cancel_timeout();
          auto lock = session->connection->handler_runner->continue_lock();
          if (!lock) {
            return;
          }
          if (!ec) {
            session->connection->set_timeout(config.timeout_connect);
            asio::async_connect(
                *session->connection->socket,
                results,
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
                    session->connection->socket->set_option(option, ec);
                    this->write(session);
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

} // namespace crystal
