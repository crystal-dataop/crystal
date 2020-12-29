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

#include <librdkafka/rdkafkacpp.h>

#include "crystal/bridge/kafka/DeliveryReportCallback.h"
#include "crystal/bridge/kafka/EventCallback.h"

namespace crystal {
namespace kafka {

class TopicProducer {
 public:
  TopicProducer(const std::string& broker,
                const std::string& topicName,
                int32_t partition = RdKafka::Topic::PARTITION_UA);
  virtual ~TopicProducer();

  bool init();

  bool produce(const std::string& message);

  int poll(int timeout);

 private:
  std::string broker_;
  std::string topicName_;
  int partition_;

  DeliveryReportCb drCallback_;
  EventCb eventCallback_;

  std::unique_ptr<RdKafka::Conf> globalConf_;
  std::unique_ptr<RdKafka::Conf> topicConf_;
  std::unique_ptr<RdKafka::Producer> producer_;
  std::unique_ptr<RdKafka::Topic> topic_;
  std::string errstr_;
};

} // namespace kafka
} // namespace crystal
