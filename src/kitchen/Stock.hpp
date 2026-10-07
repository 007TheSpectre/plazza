#pragma once

#include "../pizza/PizzaTypes.hpp"
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

/**
 * @brief Ingredient inventory for a Kitchen, with automatic timed replenishment.
 *
 * Starts with 5 units of each of the 9 ingredients. A background thread restocks
 * all ingredients to 5 every @p restockInterval milliseconds. consume() is
 * thread-safe and called concurrently by cook threads.
 */
class Stock {
public:
    /// Starts the restock thread with the given replenishment interval.
    explicit Stock(std::chrono::milliseconds restockInterval);

    /// Stops the restock thread.
    ~Stock();

    /// Atomically deducts @p ingredients from the stock. Returns false if any ingredient is depleted.
    bool consume(const std::vector<Ingredient> &ingredients);

    /// Prints current stock levels to stdout.
    void status() const;

    Stock(const Stock &) = delete;
    Stock &operator=(const Stock &) = delete;

private:
    void replenishLoop();

    mutable std::mutex                _mutex;
    std::mutex                        _cvMutex;
    std::condition_variable           _cv;
    std::map<Ingredient, int>         _items;
    std::chrono::milliseconds         _restockInterval;
    std::atomic_bool                  _running{true};
    std::thread                       _restockThread;
};
