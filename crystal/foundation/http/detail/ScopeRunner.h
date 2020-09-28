/*
 * Copyright 2020 Yeolar
 */

#pragma once

#include <atomic>
#include <memory>

namespace crystal {

/// Makes it possible to for instance cancel Asio handlers without stopping
/// asio::io_service.
class ScopeRunner {
 public:
  struct SharedLock {
    std::atomic<long>& count;

    SharedLock(std::atomic<long>& count) : count(count) {}

    SharedLock(const SharedLock&) = delete;
    SharedLock& operator=(const SharedLock&) = delete;

    ~SharedLock() noexcept {
      count.fetch_sub(1);
    }
  };

  ScopeRunner() : count_(0) {}

  /// Returns nullptr if scope should be exited, or a shared lock otherwise.
  /// The shared lock ensures that a potential destructor call is delayed until
  /// all locks are released.
  std::unique_ptr<SharedLock> continue_lock();

  /// Blocks until all shared locks are released, then prevents future shared
  /// locks.
  void stop();

 private:
  /// Scope count that is set to -1 if scopes are to be canceled.
  std::atomic<long> count_;
};

}  // namespace crystal
