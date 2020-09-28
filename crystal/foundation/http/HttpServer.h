/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include "crystal/foundation/http/ServerBase.h"

namespace crystal {

using HTTP = asio::ip::tcp::socket;

class HttpServer : public ServerBase<HTTP> {
 public:
  HttpServer() : ServerBase(80) {}

 protected:
  void accept() override;
};

}  // namespace crystal
