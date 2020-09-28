/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/HttpHeader.h"

namespace crystal {

CaseInsensitiveMultimap HttpHeader::parse(std::istream& stream) {
  CaseInsensitiveMultimap result;
  std::string line;
  size_t param_end;
  while (getline(stream, line) &&
         (param_end = line.find(':')) != std::string::npos) {
    size_t value_start = param_end + 1;
    while (value_start + 1 < line.size() && line[value_start] == ' ') {
      ++value_start;
    }
    if (value_start < line.size()) {
      result.emplace(
          line.substr(0, param_end),
          line.substr(value_start,
                      line.size() - value_start -
                        (line.back() == '\r' ? 1 : 0)));
    }
  }
  return result;
}

std::pair<asio::buffers_iterator<asio::const_buffers_1>, bool>
HeaderEndMatch::operator()(
    asio::buffers_iterator<asio::const_buffers_1> begin,
    asio::buffers_iterator<asio::const_buffers_1> end) {
  auto it = begin;
  for (; it != end; ++it) {
    if (*it == '\n') {
      if (crlfcrlf_ == 1) {
        ++crlfcrlf_;
      } else if (crlfcrlf_ == 2) {
        crlfcrlf_ = 0;
      } else if (crlfcrlf_ == 3) {
        return {++it, true};
      }
      if (lflf_ == 0) {
        ++lflf_;
      } else if (lflf_ == 1) {
        return {++it, true};
      }
    } else if (*it == '\r') {
      if (crlfcrlf_ == 0) {
        ++crlfcrlf_;
      } else if (crlfcrlf_ == 2) {
        ++crlfcrlf_;
      } else {
        crlfcrlf_ = 0;
      }
      lflf_ = 0;
    } else {
      crlfcrlf_ = 0;
      lflf_ = 0;
    }
  }
  return {it, false};
}

}  // namespace crystal
