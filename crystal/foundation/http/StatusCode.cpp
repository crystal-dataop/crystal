/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/StatusCode.h"

#include <cstdlib>
#include <unordered_map>

namespace crystal {

const std::map<StatusCode, std::string>& status_code_strings() {
  static const std::map<StatusCode, std::string> status_code_strings = {
      {StatusCode::unknown, ""},
      {StatusCode::information_continue, "100 Continue"},
      {StatusCode::information_switching_protocols, "101 Switching Protocols"},
      {StatusCode::information_processing, "102 Processing"},
      {StatusCode::success_ok, "200 OK"},
      {StatusCode::success_created, "201 Created"},
      {StatusCode::success_accepted, "202 Accepted"},
      {StatusCode::success_non_authoritative_information, "203 Non-Authoritative Information"},
      {StatusCode::success_no_content, "204 No Content"},
      {StatusCode::success_reset_content, "205 Reset Content"},
      {StatusCode::success_partial_content, "206 Partial Content"},
      {StatusCode::success_multi_status, "207 Multi-Status"},
      {StatusCode::success_already_reported, "208 Already Reported"},
      {StatusCode::success_im_used, "226 IM Used"},
      {StatusCode::redirection_multiple_choices, "300 Multiple Choices"},
      {StatusCode::redirection_moved_permanently, "301 Moved Permanently"},
      {StatusCode::redirection_found, "302 Found"},
      {StatusCode::redirection_see_other, "303 See Other"},
      {StatusCode::redirection_not_modified, "304 Not Modified"},
      {StatusCode::redirection_use_proxy, "305 Use Proxy"},
      {StatusCode::redirection_switch_proxy, "306 Switch Proxy"},
      {StatusCode::redirection_temporary_redirect, "307 Temporary Redirect"},
      {StatusCode::redirection_permanent_redirect, "308 Permanent Redirect"},
      {StatusCode::client_error_bad_request, "400 Bad Request"},
      {StatusCode::client_error_unauthorized, "401 Unauthorized"},
      {StatusCode::client_error_payment_required, "402 Payment Required"},
      {StatusCode::client_error_forbidden, "403 Forbidden"},
      {StatusCode::client_error_not_found, "404 Not Found"},
      {StatusCode::client_error_method_not_allowed, "405 Method Not Allowed"},
      {StatusCode::client_error_not_acceptable, "406 Not Acceptable"},
      {StatusCode::client_error_proxy_authentication_required, "407 Proxy Authentication Required"},
      {StatusCode::client_error_request_timeout, "408 Request Timeout"},
      {StatusCode::client_error_conflict, "409 Conflict"},
      {StatusCode::client_error_gone, "410 Gone"},
      {StatusCode::client_error_length_required, "411 Length Required"},
      {StatusCode::client_error_precondition_failed, "412 Precondition Failed"},
      {StatusCode::client_error_payload_too_large, "413 Payload Too Large"},
      {StatusCode::client_error_uri_too_long, "414 URI Too Long"},
      {StatusCode::client_error_unsupported_media_type, "415 Unsupported Media Type"},
      {StatusCode::client_error_range_not_satisfiable, "416 Range Not Satisfiable"},
      {StatusCode::client_error_expectation_failed, "417 Expectation Failed"},
      {StatusCode::client_error_im_a_teapot, "418 I'm a teapot"},
      {StatusCode::client_error_misdirection_required, "421 Misdirected Request"},
      {StatusCode::client_error_unprocessable_entity, "422 Unprocessable Entity"},
      {StatusCode::client_error_locked, "423 Locked"},
      {StatusCode::client_error_failed_dependency, "424 Failed Dependency"},
      {StatusCode::client_error_upgrade_required, "426 Upgrade Required"},
      {StatusCode::client_error_precondition_required, "428 Precondition Required"},
      {StatusCode::client_error_too_many_requests, "429 Too Many Requests"},
      {StatusCode::client_error_request_header_fields_too_large, "431 Request Header Fields Too Large"},
      {StatusCode::client_error_unavailable_for_legal_reasons, "451 Unavailable For Legal Reasons"},
      {StatusCode::server_error_internal_server_error, "500 Internal Server Error"},
      {StatusCode::server_error_not_implemented, "501 Not Implemented"},
      {StatusCode::server_error_bad_gateway, "502 Bad Gateway"},
      {StatusCode::server_error_service_unavailable, "503 Service Unavailable"},
      {StatusCode::server_error_gateway_timeout, "504 Gateway Timeout"},
      {StatusCode::server_error_http_version_not_supported, "505 HTTP Version Not Supported"},
      {StatusCode::server_error_variant_also_negotiates, "506 Variant Also Negotiates"},
      {StatusCode::server_error_insufficient_storage, "507 Insufficient Storage"},
      {StatusCode::server_error_loop_detected, "508 Loop Detected"},
      {StatusCode::server_error_not_extended, "510 Not Extended"},
      {StatusCode::server_error_network_authentication_required, "511 Network Authentication Required"}
  };
  return status_code_strings;
}

StatusCode status_code(const std::string& status_code_string) {
  if (status_code_string.size() < 3) {
    return StatusCode::unknown;
  }

  auto number = status_code_string.substr(0, 3);
  if (number[0] < '0' || number[0] > '9' ||
      number[1] < '0' || number[1] > '9' ||
      number[2] < '0' || number[2] > '9') {
    return StatusCode::unknown;
  }

  class StringToStatusCode
      : public std::unordered_map<std::string, StatusCode> {
   public:
    StringToStatusCode() {
      for (auto& status_code : status_code_strings()) {
        emplace(status_code.second.substr(0, 3), status_code.first);
      }
    }
  };
  static StringToStatusCode string_to_status_code;

  auto pos = string_to_status_code.find(number);
  if (pos == string_to_status_code.end()) {
    return static_cast<StatusCode>(atoi(number.c_str()));
  }
  return pos->second;
}

const std::string& status_code(StatusCode status_code_enum) {
  auto pos = status_code_strings().find(status_code_enum);
  if (pos == status_code_strings().end()) {
    static std::string empty_string;
    return empty_string;
  }
  return pos->second;
}

} // namespace crystal
