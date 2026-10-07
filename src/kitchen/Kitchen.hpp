#pragma once

#include "../ipc/PizzaPacket.hpp"
#include "IKitchen.hpp"
#include "Stock.hpp"
#include "ThreadPool.hpp"
#include <chrono>
#include <mutex>
#include <string>

class NamedPipe;

/**
 * @brief Child-process kitchen. Reads orders from a FIFO, cooks pizzas on a
 *        thread pool, and writes results back through a second FIFO.
 *
 * Created by Reception via fork(). The process exits after 5 seconds of idle
 * (no pending orders and no new order received within the timeout window).
 */
class Kitchen : public IKitchen {
public:
    Kitchen(std::size_t numCooks,
            std::chrono::milliseconds restockInterval,
            double timeMultiplier,
            const std::string &orderPipePath,
            const std::string &resultPipePath);

    /// Enters the order-polling loop; returns when the idle timeout fires.
    void run();

    /// Prints per-cook pending count and stock status to stdout.
    void status() const override;

    Kitchen(const Kitchen &) = delete;
    Kitchen &operator=(const Kitchen &) = delete;

private:
    /// Task submitted to the ThreadPool for each incoming pizza order.
    void cookPizza(PizzaPacket packet, NamedPipe &resultPipe, std::mutex &resultMutex);

    std::size_t _numCooks;
    double      _timeMultiplier;
    Stock       _stock;
    ThreadPool  _pool;
    std::string _orderPipePath;
    std::string _resultPipePath;
};
