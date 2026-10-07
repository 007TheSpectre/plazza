#include <criterion/criterion.h>
#include "kitchen/ThreadPool.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

static void drainPool(ThreadPool &pool)
{
    while (pool.pendingCount() > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

Test(ThreadPoolStressTest, StressTest1000Tasks)
{
    ThreadPool pool(4);
    std::atomic<int> counter{0};

    for (int i = 0; i < 1000; ++i)
        pool.submit([&counter]() { ++counter; });

    drainPool(pool);
    cr_assert_eq(counter.load(), 1000);
}

Test(ThreadPoolStressTest, DestructorWaitsForTaskCompletion)
{
    std::atomic<int> counter{0};
    {
        ThreadPool pool(2);
        for (int i = 0; i < 10; ++i)
            pool.submit([&counter]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                ++counter;
            });
    }
    cr_assert_eq(counter.load(), 10);
}

Test(ThreadPoolStressTest, IsSaturatedAtExactThreshold)
{
    ThreadPool pool(1);
    std::atomic<bool> hold{true};

    pool.submit([&hold]() {
        while (hold.load(std::memory_order_relaxed))
            std::this_thread::yield();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (int i = 0; i < 4; ++i)
        pool.submit([]() {});

    cr_assert(pool.isSaturated(5));
    cr_assert_not(pool.isSaturated(6));

    hold = false;
    drainPool(pool);
}

Test(ThreadPoolStressTest, ConcurrentSubmitFrom10Threads)
{
    ThreadPool pool(4);
    std::atomic<int> counter{0};

    std::vector<std::thread> submitters;
    for (int i = 0; i < 10; ++i) {
        submitters.emplace_back([&]() {
            for (int j = 0; j < 10; ++j)
                pool.submit([&counter]() { ++counter; });
        });
    }
    for (auto &s : submitters) s.join();

    drainPool(pool);
    cr_assert_eq(counter.load(), 100);
}

Test(ThreadPoolStressTest, TaskCapturingLargeDataExecutesCorrectly)
{
    ThreadPool pool(2);
    std::vector<uint8_t> large(1024 * 100, 0xAB);
    std::atomic<bool> verified{false};

    pool.submit([data = large, &verified]() {
        verified = !data.empty() && data[0] == 0xAB && data.back() == 0xAB;
    });

    drainPool(pool);
    cr_assert(verified.load());
}
