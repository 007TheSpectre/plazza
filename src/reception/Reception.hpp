#pragma once

#include "KitchenProxy.hpp"
#include "Parser.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

/**
 * @brief Main process controller: interactive shell, load balancer, and kitchen lifecycle manager.
 *
 * Reception owns the list of active KitchenProxy objects. It distributes
 * each individual pizza to the least-busy kitchen (or forks a new one when all
 * are saturated), and runs a background thread that polls result pipes and
 * prints completion messages to stdout.
 */
class Reception {
public:
    Reception(double timeMultiplier, std::size_t numCooks,
              std::chrono::milliseconds restockInterval);

    /// Stops the result poller thread and destroys all KitchenProxy handles (which reap child processes).
    ~Reception();

    /// Enters the stdin read loop. Blocks until EOF.
    void run();

    Reception(const Reception &) = delete;
    Reception &operator=(const Reception &) = delete;

private:
    /// Returns the kitchen with the lowest pending count, spawning one if all are saturated.
    KitchenProxy &leastBusyKitchen();

    /// Forks a new Kitchen child process and registers its KitchenProxy.
    KitchenProxy &spawnKitchen();

    /// Distributes each pizza in @p order to a kitchen one by one.
    void          distributeOrder(const Parser::Order &order);

    /// Prints the pending count for every active kitchen.
    void          printStatus() const;

    /// Background loop: polls every kitchen's result pipe at 50 ms intervals.
    void          resultPollerLoop();

    /// Removes KitchenProxy entries whose child process has exited.
    void          reapDeadKitchens();

    double                                      _timeMultiplier;
    std::size_t                                 _numCooks;
    std::chrono::milliseconds                   _restockInterval;
    std::vector<std::unique_ptr<KitchenProxy>>  _kitchens;
    mutable std::mutex                          _kitchensMutex;
    std::atomic_bool                            _running{true};
    std::thread                                 _pollThread;
    std::atomic<uint32_t>                       _orderCounter{0};
};
