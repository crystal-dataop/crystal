/*
 * Copyright 2020 Yeolar
 */

#include "crystal/foundation/http/detail/ScopeRunner.h"

#ifdef __SSE2__
#include <emmintrin.h>
#endif

namespace crystal {

#ifdef __SSE2__
inline void spin_loop_pause() noexcept {
  _mm_pause();
}
#else
inline void spin_loop_pause() noexcept {}
#endif

std::unique_ptr<ScopeRunner::SharedLock> ScopeRunner::continue_lock() {
  long expected = count_;
  while (expected >= 0 &&
         !count_.compare_exchange_weak(expected, expected + 1)) {
    spin_loop_pause();
  }

  if (expected < 0) {
    return nullptr;
  }
  return std::unique_ptr<SharedLock>(new SharedLock(count_));
}

void ScopeRunner::stop() {
  long expected = 0;
  while (!count_.compare_exchange_weak(expected, -1)) {
    if (expected < 0) {
      return;
    }
    expected = 0;
    spin_loop_pause();
  }
}

}  // namespace crystal
