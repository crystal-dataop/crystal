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

/*
 * Modified from folly.
 */

#pragma once

#include <string>
#include <thread>
#include <pthread.h>

namespace crystal {

/**
 * Get the name of the given thread.
 */
std::string getThreadName(std::thread::id tid);

/**
 * Equivalent to getThreadName(std::this_thread::get_id());
 */
std::string getCurrentThreadName();

/**
 * Set the name of the given thread.
 * Returns false on failure, if an error occurs or the functionality
 * is not available.
 */
bool setThreadName(std::thread::id tid, std::string_view name);
bool setThreadName(pthread_t pid, std::string_view name);

/**
 * Equivalent to setThreadName(std::this_thread::get_id(), name);
 */
bool setThreadName(std::string_view name);

} // namespace crystal
