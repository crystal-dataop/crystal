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

#include "crystal/bridge/kafka/EventCallback.h"
#include "crystal/bridge/kafka/KafkaClient.h"
#include "crystal/foundation/Logging.h"

namespace crystal {
namespace kafka {

void EventCb::event_cb(RdKafka::Event& event) {
  switch (event.type()) {
    case RdKafka::Event::EVENT_ERROR: {
      CRYSTAL_LOG(ERROR) << "error(" << RdKafka::err2str(event.err()) << "): "
          << event.str();
      if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN) {
        KafkaClient::getInstance().running = false;
      }
      break;
    }
    case RdKafka::Event::EVENT_STATS: {
      CRYSTAL_LOG(INFO) << "stats: " << event.str();
      break;
    }
    case RdKafka::Event::EVENT_LOG: {
      switch (event.severity()) {
        case RdKafka::Event::EVENT_SEVERITY_EMERG:
        case RdKafka::Event::EVENT_SEVERITY_ALERT:
        case RdKafka::Event::EVENT_SEVERITY_CRITICAL:
        case RdKafka::Event::EVENT_SEVERITY_ERROR:
          CRYSTAL_LOG(ERROR) << "log" << event.severity()
            << "(" << event.fac() << "): " << event.str();
          break;
        case RdKafka::Event::EVENT_SEVERITY_WARNING:
        case RdKafka::Event::EVENT_SEVERITY_NOTICE:
          CRYSTAL_LOG(WARN) << "log" << event.severity()
            << "(" << event.fac() << "): " << event.str();
          break;
        case RdKafka::Event::EVENT_SEVERITY_INFO:
          CRYSTAL_LOG(INFO) << "log" << event.severity()
            << "(" << event.fac() << "): " << event.str();
          break;
        case RdKafka::Event::EVENT_SEVERITY_DEBUG:
          CRYSTAL_LOG(DEBUG) << "log" << event.severity()
            << "(" << event.fac() << "): " << event.str();
          break;
      }
      break;
    }
    case RdKafka::Event::EVENT_THROTTLE: {
      CRYSTAL_LOG(INFO) << "throttle: " << event.throttle_time() << "ms,"
          << " broker=" << event.broker_name()
          << "(" << event.broker_id() << ")";
      break;
    }
    default:
      CRYSTAL_LOG(INFO) << "event " << event.type()
          << "(" << RdKafka::err2str(event.err()) << "): " << event.str();
      break;
  }
}

} // namespace kafka
} // namespace crystal
