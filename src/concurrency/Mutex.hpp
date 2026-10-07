#pragma once

#include "IMutex.hpp"
#include <mutex>

/// Concrete IMutex backed by std::mutex. native() exposes the underlying handle for std::unique_lock.
class Mutex : public IMutex {
public:
    void lock() override;
    void unlock() override;
    bool tryLock() override;

    /// Returns the underlying std::mutex for use with std::unique_lock / std::condition_variable.
    std::mutex &native();

private:
    std::mutex _mutex;
};
