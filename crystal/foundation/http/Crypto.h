/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <sstream>
#include <string>

namespace crystal {

class Crypto {
  static constexpr size_t buffer_size = 131072;

 public:
  /// Returns hex string from bytes in input string.
  static std::string to_hex_string(const std::string& input);

  /// Returns md5 hash value from input string.
  static std::string md5(const std::string& input, size_t iterations = 1);

  /// Returns md5 hash value from input stream.
  static std::string md5(std::istream& stream, size_t iterations = 1);

  /// Returns sha1 hash value from input string.
  static std::string sha1(const std::string& input, size_t iterations = 1);

  /// Returns sha1 hash value from input stream.
  static std::string sha1(std::istream& stream, size_t iterations = 1);

  /// Returns sha256 hash value from input string.
  static std::string sha256(const std::string& input, size_t iterations = 1);

  /// Returns sha256 hash value from input stream.
  static std::string sha256(std::istream& stream, size_t iterations = 1);

  /// Returns sha512 hash value from input string.
  static std::string sha512(const std::string& input, size_t iterations = 1);

  /// Returns sha512 hash value from input stream.
  static std::string sha512(std::istream& stream, size_t iterations = 1);

  /// Returns PBKDF2 hash value from the given password.
  static std::string pbkdf2(const std::string& password,
                            const std::string& salt,
                            int iterations,
                            int key_size);
};

}  // namespace crystal
