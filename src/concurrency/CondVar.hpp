#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>

/// Thin wrapper around std::condition_variable exposing the project's naming conventions.
class CondVar {
public:
    /// Blocks on @p lock until @p predicate returns true.
    void wait(std::unique_lock<std::mutex> &lock, std::function<bool()> predicate);

    /// Wakes one waiting thread.
    void notifyOne();

    /// Wakes all waiting threads.
    void notifyAll();

private:
    std::condition_variable _cv;
};
