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

#include <string_view>

#include "crystal/bridge/kafka/ConsumeCallback.h"
#include "crystal/bridge/kafka/KafkaClient.h"
#include "crystal/foundation/Logging.h"

namespace crystal {
namespace kafka {

void msg_consume(RdKafka::Message* message, void*) {
  switch (message->err()) {
    case RdKafka::ERR_NO_ERROR: {
      /* Real message */
      KafkaClient& client = KafkaClient::getInstance();
      client.msgCount++;
      client.msgBytes += message->len();
      CRYSTAL_LOG(DEBUG) << "message:"
          << "\n  offset=" << message->offset()
          << "\n  timestamp=" << message->timestamp().timestamp
          << "(" << message->timestamp().type << ")"
          << "\n  key=" << (message->key() ? *message->key() : "")
          << "\n  payload=" << std::string_view(
              reinterpret_cast<const char*>(message->payload()),
              message->len());
      break;
    }
    case RdKafka::ERR__PARTITION_EOF: {
      /* Last message */
      KafkaClient& client = KafkaClient::getInstance();
      if (client.exitEof && ++client.eofCount == client.partitionCount) {
        CRYSTAL_LOG(INFO) << "EOF reached for all " << client.partitionCount
            << " partition(s)";
        KafkaClient::getInstance().running = false;
      }
      break;
    }
    case RdKafka::ERR__TIMED_OUT:
      CRYSTAL_LOG(DEBUG) << "consume timeout";
      break;
    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
    default:
      CRYSTAL_LOG(ERROR) << "consume failed: " << message->errstr();
      KafkaClient::getInstance().running = false;
      break;
  }
}

}  // namespace kafka
}  // namespace crystal
