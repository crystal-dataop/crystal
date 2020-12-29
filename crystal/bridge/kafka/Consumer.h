/*
 * Copyright 2020 Yeolar
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

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <librdkafka/rdkafkacpp.h>

#include "crystal/bridge/kafka/ConsumeCallback.h"
#include "crystal/bridge/kafka/EventCallback.h"

namespace crystal {
namespace kafka {

class Consumer {
 public:
  Consumer(const std::string& groupId,
           const std::string& broker,
           const std::vector<std::string>& topics);
  virtual ~Consumer();

  bool init();

  bool consume();

 private:
  std::string groupId_;
  std::string broker_;
  std::vector<std::string> topics_;

  ConsumeCb consumeCallback_;
  EventCb eventCallback_;

  std::unique_ptr<RdKafka::Conf> globalConf_;
  std::unique_ptr<RdKafka::Conf> topicConf_;
  std::unique_ptr<RdKafka::KafkaConsumer> consumer_;
  std::string errstr_;

  bool running_{true};
};

} // namespace kafka
} // namespace crystal
