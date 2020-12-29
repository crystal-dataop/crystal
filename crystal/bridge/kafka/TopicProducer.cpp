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

#include "crystal/bridge/kafka/TopicProducer.h"
#include "crystal/foundation/Logging.h"

namespace crystal {
namespace kafka {

TopicProducer::TopicProducer(const std::string& broker,
                             const std::string& topicName,
                             int32_t partition)
    : broker_(broker), topicName_(topicName), partition_(partition) {
  globalConf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
  globalConf_->set("bootstrap.servers", broker_, errstr_);
  globalConf_->set("dr_cb", &drCallback_, errstr_);
  globalConf_->set("event_cb", &eventCallback_, errstr_);

  topicConf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
}

TopicProducer::~TopicProducer() {
  while (producer_->outq_len() > 0) {
    CRYSTAL_LOG(INFO) << topicName_ << "@" << producer_->name()
        << " waiting for outq, len=" << producer_->outq_len();
    producer_->poll(1000);
  }
}

bool TopicProducer::init() {
  RdKafka::Producer* producer =
    RdKafka::Producer::create(globalConf_.get(), errstr_);
  if (!producer) {
    CRYSTAL_LOG(ERROR) << "failed to create producer: " << errstr_;
    return false;
  }
  producer_.reset(producer);

  RdKafka::Topic* topic =
    RdKafka::Topic::create(producer, topicName_, topicConf_.get(), errstr_);
  if (!topic) {
    CRYSTAL_LOG(ERROR) << "failed to create topic: " << errstr_;
    return false;
  }
  topic_.reset(topic);

  CRYSTAL_LOG(INFO) << "create topic: " << topic->name()
      << " @" << producer->name();
  return true;
}

bool TopicProducer::produce(const std::string& message) {
  if (message.empty()) {
    producer_->poll(0);
    return true;
  }
  RdKafka::ErrorCode resp =
    producer_->produce(topic_.get(),
                       partition_,
                       RdKafka::Producer::RK_MSG_COPY,
                       const_cast<char*>(message.data()),
                       message.size(),
                       nullptr,
                       nullptr);
  producer_->poll(0);
  if (resp != RdKafka::ERR_NO_ERROR) {
    CRYSTAL_LOG(ERROR) << "produce failed: " << RdKafka::err2str(resp);
    return false;
  }
  return true;
}

int TopicProducer::poll(int timeout) {
  return producer_->poll(timeout);
}

} // namespace kafka
} // namespace crystal
