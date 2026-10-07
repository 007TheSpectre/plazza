#pragma once

#include "IMutex.hpp"

/**
 * @brief RAII mutex guard. Locks on construction, unlocks on destruction.
 *
 * All mutex usage in the project must go through ScopedLock — raw lock()/unlock()
 * calls are forbidden by the coding standard.
 */
class ScopedLock {
public:
    explicit ScopedLock(IMutex &mutex) : _mutex(mutex) { _mutex.lock(); }
    ~ScopedLock() { _mutex.unlock(); }

    ScopedLock(const ScopedLock &) = delete;
    ScopedLock &operator=(const ScopedLock &) = delete;
    ScopedLock(ScopedLock &&) = delete;
    ScopedLock &operator=(ScopedLock &&) = delete;

private:
    IMutex &_mutex;
};
