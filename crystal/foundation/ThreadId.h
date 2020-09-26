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

#include <cstdint>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace crystal {

inline uint64_t getCurrentThreadID() {
#if __APPLE__
  return uint64_t(pthread_mach_thread_np(pthread_self()));
#else
  return uint64_t(pthread_self());
#endif
}

inline uint64_t getOSThreadID() {
#if __APPLE__
  uint64_t tid;
  pthread_threadid_np(nullptr, &tid);
  return tid;
#else
  return uint64_t(syscall(SYS_gettid));
#endif
}

} // namespace crystal
