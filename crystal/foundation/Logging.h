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

#include <string.h>

#include "crystal/foundation/LogBase.h"
#include "crystal/foundation/Singleton.h"
#include "crystal/foundation/String.h"

namespace crystal {
namespace logging {

class Logger : public BaseLogger {
 public:
  Logger() : BaseLogger("crystal") {}
};

} // namespace logging
} // namespace crystal

// printf interface macro helper; do not use directly

#ifndef CRYSTAL_LOG_STREAM
#define CRYSTAL_LOG_STREAM(severity)                                           \
  (::crystal::Singleton< ::crystal::logging::Logger>::get().level() > severity)\
    ? (void)0                                                                  \
    : ::crystal::logging::LogMessageVoidify() &                                \
      ::crystal::logging::LogMessage(                                          \
        &::crystal::Singleton< ::crystal::logging::Logger>::get(),             \
        severity, __FILENAME__, __LINE__).stream()
#endif

#ifndef CRYSTAL_LOG_STREAM_TRACE
#define CRYSTAL_LOG_STREAM_TRACE(severity, traceid)                            \
  (::crystal::Singleton< ::crystal::logging::Logger>::get().level() > severity)\
    ? (void)0                                                                  \
    : ::crystal::logging::LogMessageVoidify() &                                \
      ::crystal::logging::LogMessage(                                          \
        &::crystal::Singleton< ::crystal::logging::Logger>::get(),             \
        severity, __FILENAME__, __LINE__, traceid).stream()
#endif

#ifndef CRYSTAL_LOG_STREAM_RAW
#define CRYSTAL_LOG_STREAM_RAW(severity)                                       \
  (::crystal::Singleton< ::crystal::logging::Logger>::get().level() > severity)\
    ? (void)0                                                                  \
    : ::crystal::logging::LogMessageVoidify() &                                \
      ::crystal::logging::RawLogMessage(                                       \
        &::crystal::Singleton< ::crystal::logging::Logger>::get()).stream()
#endif

#ifndef CRYSTAL_LOG_COST_IMPL
#define CRYSTAL_LOG_COST_IMPL(severity, threshold)                             \
  (::crystal::Singleton< ::crystal::logging::Logger>::get().level() > severity)\
    ? (bool)0                                                                  \
    : ::crystal::logging::detail::LogScopeParam{                               \
        &::crystal::Singleton< ::crystal::logging::Logger>::get(),             \
        severity, __FILENAME__, __LINE__, threshold} + [&]() noexcept
#endif

///////////////////////////////////////////////////////////////////////////
// crystal framework
//
#define CRYSTAL_LOG(severity) \
  CRYSTAL_LOG_STREAM(::crystal::logging::LOG_##severity)

#define CRYSTAL_TLOG(severity, traceid) \
  CRYSTAL_LOG_STREAM_TRACE(::crystal::logging::LOG_##severity, traceid)

#define CRYSTAL_PLOG(severity) \
  CRYSTAL_LOG(severity) << ::crystal::errnoStr(errno) << ", "

#define CRYSTAL_RLOG(severity) \
  CRYSTAL_LOG_STREAM_RAW(::crystal::logging::LOG_##severity)

#define CRYSTAL_LOG_ON(severity) \
  if (::crystal::Singleton< ::crystal::logging::Logger>::get().level() \
    <= ::crystal::logging::LOG_##severity)

#define CRYSTAL_LOG_IF(severity, condition) \
  (!(condition)) ? (void)0 : CRYSTAL_LOG(severity)
#define CRYSTAL_PLOG_IF(severity, condition) \
  (!(condition)) ? (void)0 : CRYSTAL_PLOG(severity)

#define CRYSTAL_CHECK(condition) \
  (condition) ? (void)0 : CRYSTAL_LOG(FATAL) << \
  "Check " #condition " failed in '" << __PRETTY_FUNCTION__ << "'. "
#define CRYSTAL_CHECK_LT(a, b) CRYSTAL_CHECK((a) <  (b))
#define CRYSTAL_CHECK_LE(a, b) CRYSTAL_CHECK((a) <= (b))
#define CRYSTAL_CHECK_GT(a, b) CRYSTAL_CHECK((a) >  (b))
#define CRYSTAL_CHECK_GE(a, b) CRYSTAL_CHECK((a) >= (b))
#define CRYSTAL_CHECK_EQ(a, b) CRYSTAL_CHECK((a) == (b))
#define CRYSTAL_CHECK_NE(a, b) CRYSTAL_CHECK((a) != (b))

#define CRYSTAL_PCHECK(condition) \
  CRYSTAL_CHECK(condition) << ::crystal::errnoStr(errno) << ", "

#ifndef DCHECK
#ifndef NDEBUG
#define DCHECK(condition) CRYSTAL_CHECK(condition)
#define DCHECK_LT(a, b)   CRYSTAL_CHECK_LT(a, b)
#define DCHECK_LE(a, b)   CRYSTAL_CHECK_LE(a, b)
#define DCHECK_GT(a, b)   CRYSTAL_CHECK_GT(a, b)
#define DCHECK_GE(a, b)   CRYSTAL_CHECK_GE(a, b)
#define DCHECK_EQ(a, b)   CRYSTAL_CHECK_EQ(a, b)
#define DCHECK_NE(a, b)   CRYSTAL_CHECK_NE(a, b)
#else
#define DCHECK(condition) while (false) CRYSTAL_CHECK(condition)
#define DCHECK_LT(a, b)   while (false) CRYSTAL_CHECK_LT(a, b)
#define DCHECK_LE(a, b)   while (false) CRYSTAL_CHECK_LE(a, b)
#define DCHECK_GT(a, b)   while (false) CRYSTAL_CHECK_GT(a, b)
#define DCHECK_GE(a, b)   while (false) CRYSTAL_CHECK_GE(a, b)
#define DCHECK_EQ(a, b)   while (false) CRYSTAL_CHECK_EQ(a, b)
#define DCHECK_NE(a, b)   while (false) CRYSTAL_CHECK_NE(a, b)
#endif
#endif

#define CRYSTAL_COST_SCOPE(severity, threshold) \
  CRYSTAL_LOG_COST_IMPL(::crystal::logging::LOG_##severity, threshold)

