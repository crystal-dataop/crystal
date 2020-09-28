/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <list>
#include <memory>
#include <asio.hpp>
#include <mutex>

#include "crystal/foundation/http/ServerSession.h"
#include "crystal/foundation/http/StatusCode.h"
#include "crystal/foundation/http/Utility.h"

namespace crystal {

/// ServerResponse class where the content of the response is sent to client
/// when the object is about to be destroyed.
template <class SocketT>
class ServerResponse
    : public std::enable_shared_from_this<ServerResponse<SocketT>>,
      public std::ostream {
 public:
  /// If set to true, force server to close the connection after the response
  /// have been sent.
  ///
  /// This is useful when implementing a HTTP/1.0-server sending content
  /// without specifying the content length.
  bool close_connection_after_response = false;

  ServerResponse(std::shared_ptr<ServerSession<SocketT>> session_,
                 long timeout_content)
      : std::ostream(nullptr),
        session(std::move(session_)),
        timeout_content_(timeout_content) {
    rdbuf(streambuf_.get());
  }

  size_t size() {
    return streambuf_->size();
  }

  void send_on_delete(
      const std::function<void(const std::error_code&)>& callback = nullptr);

  /// Send the content of the response stream to client. The callback is
  /// called when the send has completed.
  ///
  /// Use this function if you need to recursively send parts of a longer
  /// message, or when using server-sent events.
  void send(std::function<void(const std::error_code&)> callback = nullptr);

  /// Write directly to stream buffer using std::ostream::write.
  void write(const char_type* ptr, std::streamsize n);

  /// Convenience function for writing status line, potential header fields,
  /// and empty content.
  void write(
      StatusCode status_code = StatusCode::success_ok,
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap());

  /// Convenience function for writing status line, header fields, and
  /// content.
  void write(
      StatusCode status_code,
      std::string_view content,
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap());

  /// Convenience function for writing status line, header fields, and
  /// content.
  void write(
      StatusCode status_code,
      std::istream& content,
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap());

  /// Convenience function for writing success status line, header fields, and
  /// content.
  void write(
      std::string_view content,
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap()) {
    write(StatusCode::success_ok, content, header);
  }

  /// Convenience function for writing success status line, header fields, and
  /// content.
  void write(
      std::istream& content,
      const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap()) {
    write(StatusCode::success_ok, content, header);
  }

  /// Convenience function for writing success status line, and header fields.
  void write(const CaseInsensitiveMultimap& header) {
    write(StatusCode::success_ok, std::string(), header);
  }

  std::shared_ptr<ServerSession<SocketT>> session;

 private:
  template <typename size_type>
  void write_header(const CaseInsensitiveMultimap& header, size_type size);

  void send_from_queue();

  std::unique_ptr<asio::streambuf> streambuf_ =
      std::unique_ptr<asio::streambuf>(new asio::streambuf());

  long timeout_content_;

  std::mutex send_queue_mutex_;
  std::list<std::pair<std::shared_ptr<asio::streambuf>,
                      std::function<void(const std::error_code&)>>> send_queue_;
};

//////////////////////////////////////////////////////////////////////

template <class SocketT>
void ServerResponse<SocketT>::send_on_delete(
    const std::function<void(const std::error_code&)>& callback) {
  // Keep ServerResponse instance alive through the following async_write
  auto self = this->shared_from_this();

  asio::async_write(
      *session->connection->socket,
      *streambuf_,
      [self, callback](const std::error_code& ec,
                       size_t /*bytes_transferred*/) {
        auto lock = self->session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }
        if (callback) {
          callback(ec);
        }
      });
}

template <class SocketT>
void ServerResponse<SocketT>::send(
    std::function<void(const std::error_code&)> callback) {
  std::shared_ptr<asio::streambuf> streambuf = std::move(streambuf_);
  streambuf_ = std::make_unique<asio::streambuf>();
  rdbuf(streambuf_.get());

  std::lock_guard lock(send_queue_mutex_);
  send_queue_.emplace_back(std::move(streambuf), std::move(callback));
  if (send_queue_.size() == 1) {
    send_from_queue();
  }
}

template <class SocketT>
inline void ServerResponse<SocketT>::write(
    const char_type* ptr, std::streamsize n) {
  std::ostream::write(ptr, n);
}

template <class SocketT>
inline void ServerResponse<SocketT>::write(
    StatusCode status_code,
    const CaseInsensitiveMultimap& header) {
  *this << "HTTP/1.1 " << crystal::status_code(status_code) << "\r\n";
  write_header(header, 0);
}

template <class SocketT>
inline void ServerResponse<SocketT>::write(
    StatusCode status_code,
    std::string_view content,
    const CaseInsensitiveMultimap& header) {
  *this << "HTTP/1.1 " << crystal::status_code(status_code) << "\r\n";
  write_header(header, content.size());
  if (!content.empty()) {
    *this << content;
  }
}

template <class SocketT>
inline void ServerResponse<SocketT>::write(
    StatusCode status_code,
    std::istream& content,
    const CaseInsensitiveMultimap& header) {
  *this << "HTTP/1.1 " << crystal::status_code(status_code) << "\r\n";
  content.seekg(0, std::ios::end);
  auto size = content.tellg();
  content.seekg(0, std::ios::beg);
  write_header(header, size);
  if (size) {
    *this << content.rdbuf();
  }
}

template <class SocketT>
template <typename size_type>
void ServerResponse<SocketT>::write_header(
    const CaseInsensitiveMultimap& header, size_type size) {
  bool content_length_written = false;
  bool chunked_transfer_encoding = false;
  CaseInsensitiveEqual case_insensitive_equal;
  for (auto& field : header) {
    if (!content_length_written &&
        case_insensitive_equal(field.first, "content-length")) {
      content_length_written = true;
    } else if (!chunked_transfer_encoding &&
               case_insensitive_equal(field.first, "transfer-encoding") &&
               case_insensitive_equal(field.second, "chunked")) {
      chunked_transfer_encoding = true;
    }

    *this << field.first << ": " << field.second << "\r\n";
  }
  if (!content_length_written &&
      !chunked_transfer_encoding &&
      !close_connection_after_response) {
    *this << "Content-Length: " << size << "\r\n\r\n";
  } else {
    *this << "\r\n";
  }
}

template <class SocketT>
void ServerResponse<SocketT>::send_from_queue() {
  auto self = this->shared_from_this();
  asio::async_write(
      *self->session->connection->socket,
      *send_queue_.begin()->first,
      [self](const std::error_code& ec, size_t /*bytes_transferred*/) {
        auto lock = self->session->connection->handler_runner->continue_lock();
        if (!lock) {
          return;
        }
        {
          std::unique_lock<std::mutex> lock(self->send_queue_mutex_);
          if (!ec) {
            auto it = self->send_queue_.begin();
            auto callback = std::move(it->second);
            self->send_queue_.erase(it);
            if (self->send_queue_.size() > 0) {
              self->send_from_queue();
            }

            lock.unlock();
            if (callback) {
              callback(ec);
            }
          } else {
            // All handlers in the queue is called with ec:
            std::vector<std::function<void(const std::error_code&)>> callbacks;
            for (auto& pair : self->send_queue_) {
              if (pair.second) {
                callbacks.emplace_back(std::move(pair.second));
              }
            }
            self->send_queue_.clear();

            lock.unlock();
            for (auto& callback : callbacks) {
              callback(ec);
            }
          }
        }
      });
}

}  // namespace crystal
