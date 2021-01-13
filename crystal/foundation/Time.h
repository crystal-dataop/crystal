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

#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>
#include <sys/time.h>

namespace crystal {

typedef std::chrono::steady_clock Clock;

inline uint64_t nanoTimestampNow() {
  return Clock::now().time_since_epoch().count();
}

inline uint64_t nanoElapsed(uint64_t nts) {
  return nanoTimestampNow() - nts;
}

inline uint64_t timestampNow() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
      Clock::now().time_since_epoch()).count();
}

inline uint64_t elapsed(uint64_t ts) {
  return timestampNow() - ts;
}

inline uint64_t sysTimestampNow() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();
}

inline struct timeval toTimeval(uint64_t t) {
  return {
    time_t(t / 1000000),
    suseconds_t(t % 1000000)
  };
}

inline struct timespec toTimespec(uint64_t t) {
  return {
    time_t(t / 1000000000),
    long(t % 1000000000)
  };
}

std::string timePrintf(time_t t, const char *format);

inline std::string timeNowPrintf(const char *format) {
  return timePrintf(time(nullptr), format);
}

} // namespace crystal
