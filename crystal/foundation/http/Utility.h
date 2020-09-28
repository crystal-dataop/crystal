/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <string>
#include <unordered_map>

namespace crystal {

struct AsciiCaseSensitive {
  bool operator()(char lhs, char rhs) const {
    return lhs == rhs;
  }
};

/**
 * Check if two ascii characters are case insensitive equal.
 * The difference between the lower/upper case characters are the 6-th bit.
 * We also check they are alpha chars, in case of xor = 32.
 */
struct AsciiCaseInsensitive {
  bool operator()(char lhs, char rhs) const {
    char k = lhs ^ rhs;
    if (k == 0) {
      return true;
    }
    if (k != 32) {
      return false;
    }
    k = lhs | rhs;
    return (k >= 'a' && k <= 'z');
  }
};

struct CaseInsensitiveEqual {
  bool operator()(const std::string& lhs, const std::string& rhs) const {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                      AsciiCaseInsensitive());
  }
};

struct CaseInsensitiveHash {
  size_t operator()(const std::string& str) const;
};

using CaseInsensitiveMultimap =
    std::unordered_multimap<std::string, std::string, CaseInsensitiveHash,
                            CaseInsensitiveEqual>;

/// URI percent encoding and decoding
class URIPercent {
 public:
  static std::string encode(const std::string& value);
  static std::string decode(const std::string& value);
};

/// Query string creation and parsing
class QueryString {
 public:
  static std::string create(const CaseInsensitiveMultimap& fields);
  static CaseInsensitiveMultimap parse(const std::string& query_string);
};

}  // namespace crystal
