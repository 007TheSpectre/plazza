#include <criterion/criterion.h>
#include "kitchen/Stock.hpp"
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

Test(StockConcurrentTest, ConcurrentConsumeExactlyFiveSucceed)
{
    Stock stock(std::chrono::hours(24));
    std::atomic<int> successes{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < 20; ++i) {
        threads.emplace_back([&]() {
            if (stock.consume({Ingredient::Dough}))
                ++successes;
        });
    }
    for (auto &t : threads) t.join();
    cr_assert_eq(successes.load(), 5);
}

Test(StockConcurrentTest, ConcurrentConsumeMultiIngredientExactlyFiveSucceed)
{
    Stock stock(std::chrono::hours(24));
    std::atomic<int> successes{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            if (stock.consume({Ingredient::Tomato, Ingredient::Gruyere}))
                ++successes;
        });
    }
    for (auto &t : threads) t.join();
    cr_assert_eq(successes.load(), 5);
}

Test(StockConcurrentTest, ReplenishThreadJoinsCleanlyOnDestruction)
{
    std::atomic<int> consumed{0};
    {
        Stock stock(std::chrono::milliseconds(1));
        for (int i = 0; i < 5; ++i)
            stock.consume({Ingredient::Dough});
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        consumed = stock.consume({Ingredient::Dough}) ? 1 : 0;
    }
    cr_assert_eq(consumed.load(), 1);
}

Test(StockConcurrentTest, ConsumeEmptyIngredientListAlwaysSucceeds)
{
    Stock stock(std::chrono::hours(24));
    for (int i = 0; i < 10; ++i)
        cr_assert(stock.consume({}));
}

Test(StockConcurrentTest, ConsumeAndReplenishConcurrently)
{
    Stock stock(std::chrono::milliseconds(5));
    std::atomic<bool> stop{false};
    std::atomic<int> successCount{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            while (!stop.load(std::memory_order_relaxed)) {
                if (stock.consume({Ingredient::Ham}))
                    ++successCount;
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    stop = true;
    for (auto &t : threads) t.join();
    cr_assert_gt(successCount.load(), 5);
}
