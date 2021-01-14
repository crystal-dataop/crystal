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

#include <condition_variable>
#include <mutex>

namespace crystal {

class Semaphore {
 public:
  Semaphore(int count = 0) : count_(count) {}

  void post() {
    std::unique_lock<std::mutex> lock(mutex_);
    ++count_;
    cv_.notify_one();
  }

  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [=] { return count_ > 0; });
    --count_;
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  int count_;
};

} // namespace crystal
