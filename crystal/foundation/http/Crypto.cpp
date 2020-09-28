/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/Crypto.h"

#include <iomanip>
#include <vector>

#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace crystal {

std::string Crypto::to_hex_string(const std::string& input) {
  std::stringstream hex_stream;
  hex_stream << std::hex << std::internal << std::setfill('0');
  for (auto& byte : input) {
    hex_stream << std::setw(2)
               << static_cast<int>(static_cast<unsigned char>(byte));
  }
  return hex_stream.str();
}

std::string Crypto::md5(const std::string& input, size_t iterations) {
  std::string hash;
  hash.resize(128 / 8);
  MD5(reinterpret_cast<const unsigned char*>(&input[0]), input.size(),
      reinterpret_cast<unsigned char*>(&hash[0]));
  for (size_t c = 1; c < iterations; ++c) {
    MD5(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
        reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::md5(std::istream& stream, size_t iterations) {
  MD5_CTX context;
  MD5_Init(&context);
  std::streamsize read_length;
  std::vector<char> buffer(buffer_size);
  while ((read_length = stream.read(&buffer[0], buffer_size).gcount()) > 0) {
    MD5_Update(&context, buffer.data(), static_cast<size_t>(read_length));
  }
  std::string hash;
  hash.resize(128 / 8);
  MD5_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
  for (size_t c = 1; c < iterations; ++c) {
    MD5(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
        reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::sha1(const std::string& input, size_t iterations) {
  std::string hash;
  hash.resize(160 / 8);
  SHA1(reinterpret_cast<const unsigned char*>(&input[0]), input.size(),
       reinterpret_cast<unsigned char*>(&hash[0]));
  for (size_t c = 1; c < iterations; ++c) {
    SHA1(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
         reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::sha1(std::istream& stream, size_t iterations) {
  SHA_CTX context;
  SHA1_Init(&context);
  std::streamsize read_length;
  std::vector<char> buffer(buffer_size);
  while ((read_length = stream.read(&buffer[0], buffer_size).gcount()) > 0) {
    SHA1_Update(&context, buffer.data(), static_cast<size_t>(read_length));
  }
  std::string hash;
  hash.resize(160 / 8);
  SHA1_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
  for (size_t c = 1; c < iterations; ++c) {
    SHA1(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
         reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::sha256(const std::string& input, size_t iterations) {
  std::string hash;
  hash.resize(256 / 8);
  SHA256(reinterpret_cast<const unsigned char*>(&input[0]), input.size(),
         reinterpret_cast<unsigned char*>(&hash[0]));
  for (size_t c = 1; c < iterations; ++c) {
    SHA256(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
           reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::sha256(std::istream& stream, size_t iterations) {
  SHA256_CTX context;
  SHA256_Init(&context);
  std::streamsize read_length;
  std::vector<char> buffer(buffer_size);
  while ((read_length = stream.read(&buffer[0], buffer_size).gcount()) > 0) {
    SHA256_Update(&context, buffer.data(), static_cast<size_t>(read_length));
  }
  std::string hash;
  hash.resize(256 / 8);
  SHA256_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
  for (size_t c = 1; c < iterations; ++c) {
    SHA256(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
           reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::sha512(const std::string& input, size_t iterations) {
  std::string hash;
  hash.resize(512 / 8);
  SHA512(reinterpret_cast<const unsigned char*>(&input[0]), input.size(),
         reinterpret_cast<unsigned char*>(&hash[0]));
  for (size_t c = 1; c < iterations; ++c) {
    SHA512(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
           reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::sha512(std::istream& stream, size_t iterations) {
  SHA512_CTX context;
  SHA512_Init(&context);
  std::streamsize read_length;
  std::vector<char> buffer(buffer_size);
  while ((read_length = stream.read(&buffer[0], buffer_size).gcount()) > 0) {
    SHA512_Update(&context, buffer.data(), static_cast<size_t>(read_length));
  }
  std::string hash;
  hash.resize(512 / 8);
  SHA512_Final(reinterpret_cast<unsigned char*>(&hash[0]), &context);
  for (size_t c = 1; c < iterations; ++c) {
    SHA512(reinterpret_cast<const unsigned char*>(&hash[0]), hash.size(),
           reinterpret_cast<unsigned char*>(&hash[0]));
  }
  return hash;
}

std::string Crypto::pbkdf2(const std::string& password,
                           const std::string& salt,
                           int iterations,
                           int key_size) {
  std::string key;
  key.resize(static_cast<size_t>(key_size));
  PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.size(),
                         reinterpret_cast<const unsigned char*>(salt.c_str()),
                         salt.size(),
                         iterations,
                         key_size,
                         reinterpret_cast<unsigned char*>(&key[0]));
  return key;
}

}  // namespace crystal
