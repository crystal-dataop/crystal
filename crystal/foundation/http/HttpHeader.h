/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <istream>
#include <asio.hpp>

#include "crystal/foundation/http/Utility.h"

namespace crystal {

class HttpHeader {
 public:
  static CaseInsensitiveMultimap parse(std::istream& stream);
};

struct HeaderEndMatch {
  /// Match condition for asio::read_until to match both standard and
  /// non-standard HTTP header endings.
  std::pair<asio::buffers_iterator<asio::const_buffers_1>, bool> operator()(
      asio::buffers_iterator<asio::const_buffers_1> begin,
      asio::buffers_iterator<asio::const_buffers_1> end);

 private:
  int crlfcrlf_ = 0;
  int lflf_ = 0;
};

}  // namespace crystal

namespace asio {

template <>
struct is_match_condition<crystal::HeaderEndMatch> : public std::true_type {};

}  // namespace asio
