/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/Utility.h"

namespace crystal {

// Based on
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x/2595226#2595226
size_t CaseInsensitiveHash::operator()(const std::string& str) const {
  size_t h = 0;
  std::hash<int> hash;
  for (auto c : str) {
    h ^= hash(tolower(c)) + 0x9e3779b9 + (h << 6) + (h >> 2);
  }
  return h;
}

std::string URIPercent::encode(const std::string& value) {
  static auto hex_chars = "0123456789ABCDEF";

  std::string result;
  result.reserve(value.size());   // Minimum size

  for (auto& chr : value) {
    if (!((chr >= '0' && chr <= '9') ||
          (chr >= 'A' && chr <= 'Z') ||
          (chr >= 'a' && chr <= 'z') ||
          chr == '-' || chr == '.' || chr == '_' || chr == '~')) {
      result += "%";
      result += hex_chars[static_cast<unsigned char>(chr) >> 4];
      result += hex_chars[static_cast<unsigned char>(chr) & 15];
    } else {
      result += chr;
    }
  }

  return result;
}

std::string URIPercent::decode(const std::string& value) {
  std::string result;
  result.reserve(value.size() / 3 + (value.size() % 3));  // Minimum size

  for (std::size_t i = 0; i < value.size(); ++i) {
    auto& chr = value[i];
    if (chr == '%' && i + 2 < value.size()) {
      auto hex = value.substr(i + 1, 2);
      auto decoded_chr =
          static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
      result += decoded_chr;
      i += 2;
    } else if (chr == '+') {
      result += ' ';
    } else {
      result += chr;
    }
  }

  return result;
}

std::string QueryString::create(const CaseInsensitiveMultimap& fields) {
  std::string result;

  bool first = true;
  for (auto& field : fields) {
    result += (!first ? "&" : "");
    result += field.first + '=' + URIPercent::encode(field.second);
    first = false;
  }

  return result;
}

CaseInsensitiveMultimap QueryString::parse(const std::string& query_string) {
  CaseInsensitiveMultimap result;

  if (query_string.empty()) {
    return result;
  }

  size_t name_pos = 0;
  size_t name_end_pos = std::string::npos;
  size_t value_pos = std::string::npos;
  for (size_t c = 0; c < query_string.size(); ++c) {
    if (query_string[c] == '&') {
      auto name = query_string.substr(
          name_pos,
          (name_end_pos == std::string::npos ? c : name_end_pos) - name_pos);
      if (!name.empty()) {
        auto value = value_pos == std::string::npos
                   ? std::string()
                   : query_string.substr(value_pos, c - value_pos);
        result.emplace(std::move(name), URIPercent::decode(value));
      }
      name_pos = c + 1;
      name_end_pos = std::string::npos;
      value_pos = std::string::npos;
    } else if (query_string[c] == '=' && name_end_pos == std::string::npos) {
      name_end_pos = c;
      value_pos = c + 1;
    }
  }
  if (name_pos < query_string.size()) {
    auto name = query_string.substr(name_pos, (name_end_pos == std::string::npos
                                               ? std::string::npos
                                               : name_end_pos - name_pos));
    if (!name.empty()) {
      auto value = value_pos >= query_string.size()
                 ? std::string()
                 : query_string.substr(value_pos);
      result.emplace(std::move(name), URIPercent::decode(value));
    }
  }

  return result;
}

}  // namespace crystal
