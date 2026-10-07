#pragma once

#include "IThread.hpp"
#include <atomic>
#include <thread>
#include <utility>

/**
 * @brief Concrete IThread wrapping any callable.
 *
 * @tparam F Any callable type (lambda, function pointer, functor).
 *
 * The callable runs on a new std::thread. _running is set to false
 * automatically when the callable returns, before join().
 */
template<typename F>
class Thread : public IThread {
public:
    explicit Thread(F callable) : _callable(std::move(callable)), _running(false) {}

    ~Thread() override
    {
        if (_thread.joinable())
            _thread.join();
    }

    void start() override
    {
        _running = true;
        _thread = std::thread([this] {
            _callable();
            _running = false;
        });
    }

    void join() override
    {
        if (_thread.joinable())
            _thread.join();
    }

    bool isRunning() const override { return _running; }

    Thread(const Thread &) = delete;
    Thread &operator=(const Thread &) = delete;

private:
    F                _callable;
    std::thread      _thread;
    std::atomic_bool _running;
};
