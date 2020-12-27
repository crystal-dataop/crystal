/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <memory>
#include <asio.hpp>

#include "crystal/foundation/http/detail/ScopeRunner.h"

namespace crystal {

template <class SocketT>
class Connection : public std::enable_shared_from_this<Connection<SocketT>> {
 public:
  template <typename... Args>
  Connection(std::shared_ptr<ScopeRunner> handler_runner_, Args&&... args)
      : handler_runner(std::move(handler_runner_)),
        socket(new SocketT(std::forward<Args>(args)...)) {}

  void close() {
    std::error_code ec;
    socket->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
    socket->lowest_layer().cancel(ec);
  }

  void set_timeout(long seconds) {
    if (seconds == 0) {
      timer = nullptr;
      return;
    }

    timer = std::make_unique<asio::steady_timer>(
#if ASIO_VERSION >= 101300
        socket->get_executor(),
#else
        socket->get_io_context(),
#endif
        std::chrono::seconds(seconds));

    // To avoid keeping Connection instance alive longer than needed
    std::weak_ptr<Connection> self_weak(this->shared_from_this());

    timer->async_wait([self_weak](const std::error_code& ec) {
      if (!ec) {
        if (auto self = self_weak.lock()) {
          self->close();
        }
      }
    });
  }

  void cancel_timeout() {
    if (timer) {
      try {
        timer->cancel();
      } catch (...) {
      }
    }
  }

  std::shared_ptr<ScopeRunner> handler_runner;
  std::unique_ptr<SocketT> socket;
  std::unique_ptr<asio::steady_timer> timer;

  // CLIENT:
  bool in_use = false;
  bool attempt_reconnect = true;
};

}  // namespace crystal
