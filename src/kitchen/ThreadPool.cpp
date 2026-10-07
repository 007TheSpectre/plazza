#include "ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t numWorkers)
{
    _workers.reserve(numWorkers);
    for (std::size_t i = 0; i < numWorkers; ++i)
        _workers.emplace_back([this] { workerLoop(); });
}

ThreadPool::~ThreadPool()
{
    _tasks.close();
    for (auto &worker : _workers)
        if (worker.joinable())
            worker.join();
}

void ThreadPool::submit(std::function<void()> task)
{
    ++_pending;
    _tasks.push(std::move(task));
}

std::size_t ThreadPool::pendingCount() const
{
    return _pending;
}

bool ThreadPool::isSaturated(std::size_t maxPending) const
{
    return _pending >= maxPending;
}

void ThreadPool::workerLoop()
{
    while (true) {
        auto task = _tasks.pop();
        if (!task)
            break;
        task();
        --_pending;
    }
}
