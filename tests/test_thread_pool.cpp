#include <criterion/criterion.h>
#include "kitchen/ThreadPool.hpp"
#include <atomic>
#include <chrono>
#include <thread>

static void drainPool(const ThreadPool &pool)
{
    while (pool.pendingCount() > 0)
        std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

Test(ThreadPoolTest, PendingCountStartsAtZero)
{
    ThreadPool pool(2);
    cr_assert_eq(pool.pendingCount(), 0u);
}

Test(ThreadPoolTest, ExecutesSingleTask)
{
    ThreadPool pool(1);
    std::atomic<bool> ran{false};
    pool.submit([&ran] { ran = true; });
    drainPool(pool);
    cr_assert(ran.load());
}

Test(ThreadPoolTest, ExecutesAllTasks)
{
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    const int N = 20;
    for (int i = 0; i < N; ++i)
        pool.submit([&counter] { ++counter; });
    drainPool(pool);
    cr_assert_eq(counter.load(), N);
}

Test(ThreadPoolTest, IsSaturatedWhenPendingExceedsMax)
{
    ThreadPool pool(1);
    std::atomic<bool> block{true};
    for (int i = 0; i < 5; ++i)
        pool.submit([&block] { while (block) std::this_thread::yield(); });
    cr_assert(pool.isSaturated(3));
    block = false;
}

Test(ThreadPoolTest, NotSaturatedWhenUnderMax)
{
    ThreadPool pool(4);
    cr_assert_not(pool.isSaturated(10));
}

Test(ThreadPoolTest, PendingCountDropsAfterCompletion)
{
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    const int N = 10;
    for (int i = 0; i < N; ++i)
        pool.submit([&counter] { ++counter; });
    drainPool(pool);
    cr_assert_eq(pool.pendingCount(), 0u);
}
