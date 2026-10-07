#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * @brief Thread-safe FIFO queue with blocking pop and cooperative shutdown.
 *
 * Producers call push(); consumers call pop(), which blocks until an item is
 * available or close() has been called. After close(), pop() returns T{} to
 * signal workers to exit.
 *
 * @tparam T Element type. Must be default-constructible (used as sentinel on close).
 */
template<typename T>
class SafeQueue {
public:
    /// Enqueues @p value and wakes one blocked consumer.
    void push(T value)
    {
        {
            std::unique_lock lock(_mutex);
            _queue.push(std::move(value));
        }
        _cv.notify_one();
    }

    /// Non-blocking dequeue. Returns false and leaves @p out unchanged if empty.
    bool tryPop(T &out)
    {
        std::unique_lock lock(_mutex);
        if (_queue.empty())
            return false;
        out = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    /// Blocking dequeue. Returns T{} if the queue is closed and empty.
    T pop()
    {
        std::unique_lock lock(_mutex);
        _cv.wait(lock, [this] { return !_queue.empty() || _closed; });
        if (_closed && _queue.empty())
            return T{};
        T value = std::move(_queue.front());
        _queue.pop();
        return value;
    }

    /// Returns true if the queue currently has no elements.
    bool empty() const
    {
        std::unique_lock lock(_mutex);
        return _queue.empty();
    }

    /// Returns the current number of enqueued elements.
    std::size_t size() const
    {
        std::unique_lock lock(_mutex);
        return _queue.size();
    }

    /// Marks the queue as closed and unblocks all waiting consumers.
    void close()
    {
        {
            std::unique_lock lock(_mutex);
            _closed = true;
        }
        _cv.notify_all();
    }

    /// Returns true if close() has been called.
    bool isClosed() const
    {
        std::unique_lock lock(_mutex);
        return _closed;
    }

private:
    mutable std::mutex      _mutex;
    std::condition_variable _cv;
    std::queue<T>           _queue;
    bool                    _closed{false};
};
