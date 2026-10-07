#include "Mutex.hpp"

void Mutex::lock()
{
    _mutex.lock();
}

void Mutex::unlock()
{
    _mutex.unlock();
}

bool Mutex::tryLock()
{
    return _mutex.try_lock();
}

std::mutex &Mutex::native()
{
    return _mutex;
}
