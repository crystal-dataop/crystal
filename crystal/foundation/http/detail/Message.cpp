/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/detail/Message.h"

#include "crystal/foundation/http/HttpHeader.h"

namespace crystal {

bool RequestMessage::parse(std::istream& stream,
                           std::string& method,
                           std::string& path,
                           std::string& query_string,
                           std::string& version,
                           CaseInsensitiveMultimap& header) {
  std::string line;
  size_t method_end;
  if (getline(stream, line) &&
      (method_end = line.find(' ')) != std::string::npos) {
    method = line.substr(0, method_end);

    size_t query_start = std::string::npos;
    size_t path_and_query_string_end = std::string::npos;
    for (size_t i = method_end + 1; i < line.size(); ++i) {
      if (line[i] == '?' && (i + 1) < line.size() &&
          query_start == std::string::npos) {
        query_start = i + 1;
      } else if (line[i] == ' ') {
        path_and_query_string_end = i;
        break;
      }
    }
    if (path_and_query_string_end != std::string::npos) {
      if (query_start != std::string::npos) {
        path = line.substr(method_end + 1, query_start - method_end - 2);
        query_string = line.substr(query_start,
                                   path_and_query_string_end - query_start);
      } else {
        path = line.substr(method_end + 1,
                           path_and_query_string_end - method_end - 1);
      }

      size_t protocol_end;
      if ((protocol_end = line.find('/', path_and_query_string_end + 1)) !=
          std::string::npos) {
        if (line.compare(path_and_query_string_end + 1,
                         protocol_end - path_and_query_string_end - 1,
                         "HTTP") != 0) {
          return false;
        }
        version = line.substr(protocol_end + 1, line.size() - protocol_end - 2);
      } else {
        return false;
      }

      header = HttpHeader::parse(stream);
    } else {
      return false;
    }
  } else {
    return false;
  }
  return true;
}

bool ResponseMessage::parse(std::istream& stream,
                            std::string& version,
                            std::string& status_code,
                            CaseInsensitiveMultimap& header) {
  std::string line;
  size_t version_end;
  if (getline(stream, line) &&
      (version_end = line.find(' ')) != std::string::npos) {
    if (5 < line.size()) {
      version = line.substr(5, version_end - 5);
    } else {
      return false;
    }
    if ((version_end + 1) < line.size()) {
      status_code = line.substr(version_end + 1,
                                line.size() - (version_end + 1) -
                                  (line.back() == '\r' ? 1 : 0));
    } else {
      return false;
    }

    header = HttpHeader::parse(stream);
  } else {
    return false;
  }
  return true;
}

}  // namespace crystal
