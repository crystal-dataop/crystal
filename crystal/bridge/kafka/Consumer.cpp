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

#include <vector>

#include "crystal/bridge/kafka/Consumer.h"
#include "crystal/foundation/Logging.h"

namespace crystal {
namespace kafka {

Consumer::Consumer(const std::string& groupId,
                   const std::string& broker,
                   const std::vector<std::string>& topics)
    : groupId_(groupId), broker_(broker), topics_(topics) {
  globalConf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
  globalConf_->set("group.id", groupId, errstr_);
  globalConf_->set("bootstrap.servers", broker_, errstr_);
  globalConf_->set("consume_cb", &consumeCallback_, errstr_);
  globalConf_->set("event_cb", &eventCallback_, errstr_);

  topicConf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
  globalConf_->set("default_topic_conf", topicConf_.get(), errstr_);
}

Consumer::~Consumer() {
  consumer_->close();
}

bool Consumer::init() {
  RdKafka::KafkaConsumer* consumer =
    RdKafka::KafkaConsumer::create(globalConf_.get(), errstr_);
  if (!consumer) {
    CRYSTAL_LOG(ERROR) << "failed to create consumer: " << errstr_;
    return false;
  }
  CRYSTAL_LOG(INFO) << "create consumer: " << consumer->name();
  consumer_.reset(consumer);

  RdKafka::ErrorCode err = consumer_->subscribe(topics_);
  if (err) {
    CRYSTAL_LOG(ERROR) << "failed to subscribe topics: "
        << RdKafka::err2str(err);
    return false;
  }
  return true;
}

bool Consumer::consume() {
  while (running_) {
    RdKafka::Message* message = consumer_->consume(5000);
    msg_consume(message, nullptr);
    delete message;
  }
  return true;
}

} // namespace kafka
} // namespace crystal
