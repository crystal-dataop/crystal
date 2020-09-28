/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <istream>

#include "crystal/foundation/http/Utility.h"

namespace crystal {

class RequestMessage {
 public:
  /** Parse request line and header fields from a request stream.
   *
   * @param[in]  stream       Stream to parse.
   * @param[out] method       HTTP method.
   * @param[out] path         Path from request URI.
   * @param[out] query_string Query string from request URI.
   * @param[out] version      HTTP version.
   * @param[out] header       Header fields.
   *
   * @return True if stream is parsed successfully, false if not.
   */
  static bool parse(std::istream& stream,
                    std::string& method,
                    std::string& path,
                    std::string& query_string,
                    std::string& version,
                    CaseInsensitiveMultimap& header);
};

class ResponseMessage {
 public:
  /** Parse status line and header fields from a response stream.
   *
   * @param[in]  stream      Stream to parse.
   * @param[out] version     HTTP version.
   * @param[out] status_code HTTP status code.
   * @param[out] header      Header fields.
   *
   * @return True if stream is parsed successfully, false if not.
   */
  static bool parse(std::istream& stream,
                    std::string& version,
                    std::string& status_code,
                    CaseInsensitiveMultimap& header);
};

}  // namespace crystal
