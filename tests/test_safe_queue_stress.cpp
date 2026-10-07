#include <criterion/criterion.h>
#include "concurrency/SafeQueue.hpp"
#include <atomic>
#include <thread>
#include <vector>

Test(SafeQueueStressTest, MultipleConcurrentProducers)
{
    SafeQueue<int> q;
    std::vector<std::thread> producers;
    for (int t = 0; t < 5; ++t) {
        producers.emplace_back([&q, t]() {
            for (int i = 1; i <= 20; ++i)
                q.push(t * 20 + i);
        });
    }
    for (auto &p : producers) p.join();

    long long sum = 0;
    int count = 0;
    int val;
    while (q.tryPop(val)) {
        sum += val;
        ++count;
    }
    cr_assert_eq(count, 100);
    cr_assert_eq(sum, 5050LL);
}

Test(SafeQueueStressTest, MultipleConcurrentConsumers)
{
    SafeQueue<int> q;
    for (int i = 1; i <= 100; ++i) q.push(i);

    std::atomic<long long> sum{0};
    std::atomic<int> count{0};
    std::vector<std::thread> consumers;
    for (int t = 0; t < 5; ++t) {
        consumers.emplace_back([&]() {
            int val;
            while (q.tryPop(val)) {
                sum += val;
                ++count;
            }
        });
    }
    for (auto &c : consumers) c.join();
    cr_assert_eq(count.load(), 100);
    cr_assert_eq(sum.load(), 5050LL);
}

Test(SafeQueueStressTest, MultiProducerMultiConsumer)
{
    SafeQueue<int> q;
    std::atomic<long long> sum{0};
    std::atomic<int> count{0};

    std::vector<std::thread> producers;
    for (int t = 0; t < 4; ++t) {
        producers.emplace_back([&q, t]() {
            for (int i = 1; i <= 25; ++i)
                q.push(t * 25 + i);
        });
    }
    for (auto &p : producers) p.join();
    q.close();

    std::vector<std::thread> consumers;
    for (int t = 0; t < 4; ++t) {
        consumers.emplace_back([&]() {
            int val;
            while (q.tryPop(val)) {
                sum += val;
                ++count;
            }
        });
    }
    for (auto &c : consumers) c.join();
    cr_assert_eq(count.load(), 100);
    cr_assert_eq(sum.load(), 5050LL);
}

Test(SafeQueueStressTest, PopOnClosedEmptyReturnsDefault)
{
    SafeQueue<int> q;
    q.close();
    cr_assert_eq(q.pop(), 0);
}

Test(SafeQueueStressTest, RepeatPopOnClosedEmptyReturnsDefault)
{
    SafeQueue<int> q;
    q.close();
    for (int i = 0; i < 10; ++i)
        cr_assert_eq(q.pop(), 0);
}

Test(SafeQueueStressTest, StressHighConcurrency)
{
    SafeQueue<int> q;
    std::atomic<long long> sum{0};

    std::vector<std::thread> producers;
    for (int t = 0; t < 10; ++t) {
        producers.emplace_back([&q, t]() {
            for (int i = 1; i <= 100; ++i)
                q.push(t * 100 + i);
        });
    }
    for (auto &p : producers) p.join();
    q.close();

    std::vector<std::thread> consumers;
    for (int t = 0; t < 10; ++t) {
        consumers.emplace_back([&]() {
            int val;
            while (q.tryPop(val))
                sum += val;
        });
    }
    for (auto &c : consumers) c.join();
    cr_assert_eq(sum.load(), 500500LL);
}
