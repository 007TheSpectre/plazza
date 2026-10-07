#include "CondVar.hpp"

void CondVar::wait(std::unique_lock<std::mutex> &lock, std::function<bool()> predicate)
{
    _cv.wait(lock, predicate);
}

void CondVar::notifyOne()
{
    _cv.notify_one();
}

void CondVar::notifyAll()
{
    _cv.notify_all();
}
