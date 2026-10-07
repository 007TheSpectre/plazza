#include "Kitchen.hpp"
#include "../ipc/NamedPipe.hpp"
#include "../pizza/PizzaFactory.hpp"
#include <chrono>
#include <csignal>
#include <iostream>
#include <mutex>
#include <thread>

static constexpr int IDLE_TIMEOUT_S  = 5;
static constexpr int POLL_TIMEOUT_MS = 100;

Kitchen::Kitchen(std::size_t numCooks,
                 std::chrono::milliseconds restockInterval,
                 double timeMultiplier,
                 const std::string &orderPipePath,
                 const std::string &resultPipePath)
    : _numCooks(numCooks),
      _timeMultiplier(timeMultiplier),
      _stock(restockInterval),
      _pool(numCooks),
      _orderPipePath(orderPipePath),
      _resultPipePath(resultPipePath)
{
}

void Kitchen::run()
{
    signal(SIGPIPE, SIG_IGN);
    NamedPipe orderPipe(_orderPipePath, PipeMode::ReadOnly);
    NamedPipe resultPipe(_resultPipePath, PipeMode::WriteOnly);
    std::mutex resultMutex;

    auto lastActivity = std::chrono::steady_clock::now();

    while (true) {
        if (!orderPipe.hasData(POLL_TIMEOUT_MS)) {
            auto idle = std::chrono::steady_clock::now() - lastActivity;
            if (idle >= std::chrono::seconds(IDLE_TIMEOUT_S) && _pool.pendingCount() == 0)
                break;
            continue;
        }

        auto data = orderPipe.receive();
        if (data.empty())
            break;

        PizzaPacket packet = unpack(data);
        lastActivity       = std::chrono::steady_clock::now();

        _pool.submit([this, packet, &resultPipe, &resultMutex] {
            cookPizza(packet, resultPipe, resultMutex);
        });
    }

    while (_pool.pendingCount() > 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Kitchen::cookPizza(PizzaPacket packet, NamedPipe &resultPipe, std::mutex &resultMutex)
{
    auto pizza = PizzaFactory::create(packet.type, packet.size);

    while (!_stock.consume(pizza->ingredients()))
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto bakeMs = std::chrono::milliseconds(
        static_cast<long long>(pizza->bakeTime() * _timeMultiplier * 1000.0));
    std::this_thread::sleep_for(bakeMs);

    std::unique_lock lock(resultMutex);
    resultPipe << packet;
}

void Kitchen::status() const
{
    std::cout << "  [Kitchen] cooks=" << _numCooks
              << " pending=" << _pool.pendingCount() << "\n";
    _stock.status();
}
