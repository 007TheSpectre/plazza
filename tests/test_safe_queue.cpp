#include <criterion/criterion.h>
#include "concurrency/SafeQueue.hpp"
#include <atomic>
#include <chrono>
#include <thread>

Test(SafeQueueTest, PushThenPopRetrievesValue)
{
    SafeQueue<int> q;
    q.push(42);
    cr_assert_eq(q.pop(), 42);
}

Test(SafeQueueTest, TryPopOnEmptyReturnsFalse)
{
    SafeQueue<int> q;
    int val = 0;
    cr_assert_not(q.tryPop(val));
}

Test(SafeQueueTest, TryPopOnFilledReturnsTrueAndValue)
{
    SafeQueue<int> q;
    q.push(7);
    int val = 0;
    cr_assert(q.tryPop(val));
    cr_assert_eq(val, 7);
}

Test(SafeQueueTest, SizeReflectsPushCount)
{
    SafeQueue<int> q;
    cr_assert_eq(q.size(), 0u);
    q.push(1);
    q.push(2);
    cr_assert_eq(q.size(), 2u);
}

Test(SafeQueueTest, EmptyAfterAllItemsPopped)
{
    SafeQueue<int> q;
    q.push(1);
    q.pop();
    cr_assert(q.empty());
}

Test(SafeQueueTest, FIFOOrdering)
{
    SafeQueue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);
    cr_assert_eq(q.pop(), 10);
    cr_assert_eq(q.pop(), 20);
    cr_assert_eq(q.pop(), 30);
}

Test(SafeQueueTest, IsClosedAfterClose)
{
    SafeQueue<int> q;
    cr_assert_not(q.isClosed());
    q.close();
    cr_assert(q.isClosed());
}

Test(SafeQueueTest, CloseUnblocksBlockedPop)
{
    SafeQueue<int> q;
    std::thread closer([&q] {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        q.close();
    });
    int val = q.pop();
    closer.join();
    cr_assert_eq(val, 0);
}

Test(SafeQueueTest, PopFromClosedNonEmptyQueue)
{
    SafeQueue<int> q;
    q.push(42);
    q.close();
    cr_assert_eq(q.pop(), 42);
}

Test(SafeQueueTest, ProducerConsumerPreservesAllValues)
{
    SafeQueue<int> q;
    std::atomic<int> sum{0};
    std::atomic<bool> done{false};
    const int N = 100;

    std::thread consumer([&] {
        while (!done || !q.empty()) {
            int val;
            if (q.tryPop(val))
                sum += val;
            else
                std::this_thread::yield();
        }
    });

    for (int i = 1; i <= N; ++i)
        q.push(i);
    done = true;
    consumer.join();

    cr_assert_eq(sum.load(), N * (N + 1) / 2);
}
