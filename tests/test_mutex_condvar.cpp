#include <criterion/criterion.h>
#include "concurrency/CondVar.hpp"
#include "concurrency/Mutex.hpp"
#include "concurrency/ScopedLock.hpp"
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

Test(MutexTest, TryLockSucceedsWhenFree)
{
    Mutex m;
    cr_assert(m.tryLock());
    m.unlock();
}

Test(MutexTest, TryLockFailsWhenLockedByOtherThread)
{
    Mutex m;
    m.lock();
    std::atomic<bool> result{true};
    std::thread t([&]() { result = m.tryLock(); });
    t.join();
    cr_assert_not(result.load());
    m.unlock();
}

Test(MutexTest, NativeHandleLocksOtherThreadOut)
{
    Mutex m;
    std::atomic<bool> result{true};
    std::lock_guard<std::mutex> lk(m.native());
    std::thread t([&]() { result = m.tryLock(); });
    t.join();
    cr_assert_not(result.load());
}

Test(MutexTest, ScopedLockReleasesOnExit)
{
    Mutex m;
    {
        ScopedLock lk(m);
        cr_assert_not(m.tryLock());
    }
    cr_assert(m.tryLock());
    m.unlock();
}

Test(CondVarTest, NotifyOneWakesThread)
{
    Mutex m;
    CondVar cv;
    bool ready = false;
    std::atomic<bool> woke{false};

    std::thread t([&]() {
        std::unique_lock<std::mutex> lk(m.native());
        cv.wait(lk, [&]() { return ready; });
        woke = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    cr_assert_not(woke.load());
    {
        std::lock_guard<std::mutex> lk(m.native());
        ready = true;
    }
    cv.notifyOne();
    t.join();
    cr_assert(woke.load());
}

Test(CondVarTest, NotifyAllWakesAllThreads)
{
    Mutex m;
    CondVar cv;
    bool ready = false;
    std::atomic<int> woke{0};

    auto waiter = [&]() {
        std::unique_lock<std::mutex> lk(m.native());
        cv.wait(lk, [&]() { return ready; });
        ++woke;
    };

    std::thread t1(waiter), t2(waiter), t3(waiter);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    {
        std::lock_guard<std::mutex> lk(m.native());
        ready = true;
    }
    cv.notifyAll();
    t1.join();
    t2.join();
    t3.join();
    cr_assert_eq(woke.load(), 3);
}

Test(CondVarTest, NotifyWithFalsePredicateDoesNotWake)
{
    Mutex m;
    CondVar cv;
    bool ready = false;
    std::atomic<bool> woke{false};

    std::thread t([&]() {
        std::unique_lock<std::mutex> lk(m.native());
        cv.wait(lk, [&]() { return ready; });
        woke = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    cv.notifyOne();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    cr_assert_not(woke.load());

    {
        std::lock_guard<std::mutex> lk(m.native());
        ready = true;
    }
    cv.notifyOne();
    t.join();
    cr_assert(woke.load());
}
