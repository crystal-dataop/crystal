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

#include "crystal/foundation/ThreadName.h"

#include <array>
#include <cstring>
#include <type_traits>

namespace crystal {

namespace {

pthread_t stdTidToPthreadId(std::thread::id tid) {
  pthread_t id;
  std::memcpy(&id, &tid, sizeof(id));
  return id;
}

} // namespace

static constexpr size_t kMaxThreadNameLength = 16;

std::string getThreadName(std::thread::id id) {
#if defined(__GLIBC__) || defined(__APPLE__)
  std::array<char, kMaxThreadNameLength> buf;
  if (pthread_getname_np(stdTidToPthreadId(id), buf.data(), buf.size()) != 0) {
    return "";
  }
  return std::string(buf.data());
#else
  return "";
#endif
}

std::string getCurrentThreadName() {
  return getThreadName(std::this_thread::get_id());
}

bool setThreadName(std::thread::id tid, std::string_view name) {
  name = name.substr(0, kMaxThreadNameLength - 1);
  char buf[kMaxThreadNameLength] = {};
  std::memcpy(buf, name.data(), name.size());
  auto id = stdTidToPthreadId(tid);
#if defined(__GLIBC__)
  return 0 == pthread_setname_np(id, buf);
#elif defined(__APPLE__)
  if (pthread_equal(pthread_self(), id)) {
    return 0 == pthread_setname_np(buf);
  }
  return false;
#else
  (void)id;
  return false;
#endif
}

bool setThreadName(pthread_t pid, std::string_view name) {
  std::thread::id id;
  std::memcpy(static_cast<void*>(&id), &pid, sizeof(id));
  return setThreadName(id, name);
}

bool setThreadName(std::string_view name) {
  return setThreadName(std::this_thread::get_id(), name);
}

} // namespace crystal
