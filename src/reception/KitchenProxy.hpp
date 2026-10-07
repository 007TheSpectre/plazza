#pragma once

#include "../ipc/NamedPipe.hpp"
#include "../ipc/PizzaPacket.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <sys/types.h>

/**
 * @brief Reception-side handle for a running Kitchen child process.
 *
 * Owns the two NamedPipe ends (order + result) and tracks how many pizzas have
 * been sent but not yet acknowledged. Reception uses pendingCount() to decide
 * whether to spawn a new Kitchen or reuse this one.
 */
class KitchenProxy {
public:
    KitchenProxy(pid_t pid,
                 const std::string &orderPipePath,
                 const std::string &resultPipePath,
                 std::size_t numCooks);

    /// Closes pipes and reaps the child process with waitpid().
    ~KitchenProxy();

    /// Serialises @p packet and writes it to the order pipe.
    void        send(const PizzaPacket &packet);

    /// Non-blocking poll; fills @p out and decrements pending count if a result is available.
    bool        pollResult(PizzaPacket &out);

    /// Returns true if the child process is still running.
    bool        isAlive() const;

    /// Returns the number of orders sent but not yet returned as results.
    std::size_t pendingCount() const;

    /// Returns the PID of the Kitchen child process.
    pid_t       pid() const;

    KitchenProxy(const KitchenProxy &) = delete;
    KitchenProxy &operator=(const KitchenProxy &) = delete;

private:
    pid_t                      _pid;
    std::string                _orderPipePath;
    std::string                _resultPipePath;
    std::unique_ptr<NamedPipe> _orderPipe;
    std::unique_ptr<NamedPipe> _resultPipe;
    std::atomic<std::size_t>   _pending{0};
    std::size_t                _numCooks;
};
