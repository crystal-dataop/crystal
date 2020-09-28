/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <istream>
#include <asio.hpp>

namespace crystal {

class Content : public std::istream {
 public:
  Content(asio::streambuf& streambuf)
      : std::istream(&streambuf), streambuf_(streambuf) {}

  size_t size() const {
    return streambuf_.size();
  }

  std::string string() const {
    return std::string(asio::buffers_begin(streambuf_.data()),
                       asio::buffers_end(streambuf_.data()));
  }

  /// CLIENT: When true, this is the last response content part from server
  /// for the current request.
  bool end = true;

 private:
  asio::streambuf& streambuf_;
};

}  // namespace crystal
