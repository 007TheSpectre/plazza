#pragma once

/// Abstract mutex interface. Implemented by Mutex; used by ScopedLock for RAII locking.
class IMutex {
public:
    virtual ~IMutex() = default;

    /// Blocks until the mutex is acquired.
    virtual void lock() = 0;

    /// Releases the mutex.
    virtual void unlock() = 0;

    /// Acquires the mutex without blocking. Returns false if already held.
    virtual bool tryLock() = 0;
};
