#pragma once

#include "../concurrency/SafeQueue.hpp"
#include <atomic>
#include <cstddef>
#include <functional>
#include <thread>
#include <vector>

/**
 * @brief Fixed-size worker thread pool backed by a SafeQueue.
 *
 * Tasks submitted via submit() are executed by one of the N worker threads in
 * FIFO order. pendingCount() tracks submitted tasks that have not yet completed,
 * which Reception uses to decide when to spawn a new Kitchen.
 */
class ThreadPool {
public:
    /// Spawns @p numWorkers threads that start pulling from the task queue immediately.
    explicit ThreadPool(std::size_t numWorkers);

    /// Closes the task queue and joins all worker threads.
    ~ThreadPool();

    /// Enqueues @p task for execution by the next available worker.
    void submit(std::function<void()> task);

    /// Returns the number of tasks submitted but not yet finished.
    std::size_t pendingCount() const;

    /// Returns true if pendingCount() >= @p maxPending.
    bool isSaturated(std::size_t maxPending) const;

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

private:
    void workerLoop();

    SafeQueue<std::function<void()>> _tasks;
    std::vector<std::thread>         _workers;
    std::atomic<std::size_t>         _pending{0};
};
