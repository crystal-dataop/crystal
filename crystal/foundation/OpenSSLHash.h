/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Modified from folly.
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <openssl/evp.h>

#include "crystal/foundation/Likely.h"

namespace crystal {

template <typename T, void (*f)(T*)>
struct static_function_deleter {
  void operator()(T* t) const {
    f(t);
  }
};

#define DEFINE_SSL_PTR_TYPE(alias, object, deleter)                           \
  using alias##Deleter = crystal::static_function_deleter<object, &deleter>;  \
  using alias##UniquePtr = std::unique_ptr<object, alias##Deleter>

DEFINE_SSL_PTR_TYPE(EvpMdCtx, EVP_MD_CTX, EVP_MD_CTX_free);

#undef DEFINE_SSL_PTR_TYPE

class OpenSSLHash {
 public:
  class Digest {
   public:
    Digest() : ctx_(EVP_MD_CTX_new()) {}

    Digest(const Digest& other) {
      ctx_ = EvpMdCtxUniquePtr(EVP_MD_CTX_new());
      if (other.md_ != nullptr) {
        hash_init(other.md_);
        check_libssl_result(
            1, EVP_MD_CTX_copy_ex(ctx_.get(), other.ctx_.get()));
      }
    }

    Digest& operator=(const Digest& other) {
      this->~Digest();
      return *new (this) Digest(other);
    }

    void hash_init(const EVP_MD* md) {
      md_ = md;
      check_libssl_result(1, EVP_DigestInit_ex(ctx_.get(), md, nullptr));
    }
    void hash_update(std::string_view data) {
      check_libssl_result(
          1, EVP_DigestUpdate(ctx_.get(), data.data(), data.size()));
    }
    void hash_final(std::string& out) {
      const auto size = EVP_MD_size(md_);
      out.resize(size);
      uint8_t* data = reinterpret_cast<uint8_t*>(&out[0]);
      unsigned int len = 0;
      check_libssl_result(1, EVP_DigestFinal_ex(ctx_.get(), data, &len));
      check_libssl_result(size, int(len));
      md_ = nullptr;
    }

   private:
    const EVP_MD* md_ = nullptr;
    EvpMdCtxUniquePtr ctx_{nullptr};
  };

  static void hash(std::string& out, const EVP_MD* md, std::string_view data) {
    Digest hash;
    hash.hash_init(md);
    hash.hash_update(data);
    hash.hash_final(out);
  }
  static void md5(std::string& out, std::string_view data) {
    hash(out, EVP_md5(), data);
  }
  static void sha1(std::string& out, std::string_view data) {
    hash(out, EVP_sha1(), data);
  }
  static void sha256(std::string& out, std::string_view data) {
    hash(out, EVP_sha256(), data);
  }

 private:
  static inline void check_libssl_result(int expected, int result) {
    if (CRYSTAL_UNLIKELY(result != expected)) {
      throw std::runtime_error("openssl crypto function failed");
    }
  }
};

} // namespace crystal
