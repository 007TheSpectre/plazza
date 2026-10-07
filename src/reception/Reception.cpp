#include "Reception.hpp"
#include "../kitchen/Kitchen.hpp"
#include "../ipc/NamedPipe.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

static const std::map<PizzaType, std::string> PIZZA_NAMES = {
    {PizzaType::Regina,    "Regina"},
    {PizzaType::Margarita, "Margarita"},
    {PizzaType::Americana, "Americana"},
    {PizzaType::Fantasia,  "Fantasia"}
};

static const std::map<PizzaSize, std::string> SIZE_NAMES = {
    {PizzaSize::S,   "S"},
    {PizzaSize::M,   "M"},
    {PizzaSize::L,   "L"},
    {PizzaSize::XL,  "XL"},
    {PizzaSize::XXL, "XXL"}
};

Reception::Reception(double timeMultiplier, std::size_t numCooks,
                     std::chrono::milliseconds restockInterval)
    : _timeMultiplier(timeMultiplier),
      _numCooks(numCooks),
      _restockInterval(restockInterval)
{
    _pollThread = std::thread([this] { resultPollerLoop(); });
}

Reception::~Reception()
{
    _running = false;
    if (_pollThread.joinable())
        _pollThread.join();
    std::unique_lock lock(_kitchensMutex);
    _kitchens.clear();
}

void Reception::run()
{
    std::cout << "Plazza reception ready. Type orders or 'status'.\n";
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            continue;
        if (line == "status") {
            printStatus();
            continue;
        }
        try {
            auto order = Parser::parse(line);
            distributeOrder(order);
        } catch (const std::exception &e) {
            std::cerr << "Order error: " << e.what() << "\n";
        }
    }
}

KitchenProxy &Reception::spawnKitchen()
{
    static int kitchenId = 0;
    ++kitchenId;

    std::string orderPath  = "/tmp/plazza_order_"  + std::to_string(kitchenId);
    std::string resultPath = "/tmp/plazza_result_" + std::to_string(kitchenId);

    NamedPipe::create(orderPath);
    NamedPipe::create(resultPath);

    std::cout.flush();
    std::cerr.flush();
    pid_t pid = ::fork();
    if (pid < 0)
        throw std::runtime_error("fork() failed");

    if (pid == 0) {
        {
            Kitchen kitchen(_numCooks, _restockInterval, _timeMultiplier, orderPath, resultPath);
            kitchen.run();
        }
        std::exit(0);
    }

    auto proxy = std::make_unique<KitchenProxy>(pid, orderPath, resultPath, _numCooks);
    _kitchens.push_back(std::move(proxy));
    return *_kitchens.back();
}

KitchenProxy &Reception::leastBusyKitchen()
{
    reapDeadKitchens();
    const std::size_t maxPending = 2 * _numCooks;

    KitchenProxy *best = nullptr;
    for (auto &k : _kitchens) {
        if (k->pendingCount() < maxPending) {
            if (!best || k->pendingCount() < best->pendingCount())
                best = k.get();
        }
    }
    if (!best)
        return spawnKitchen();
    return *best;
}

void Reception::distributeOrder(const Parser::Order &order)
{
    std::unique_lock lock(_kitchensMutex);
    for (auto &[type, size] : order) {
        KitchenProxy &kitchen = leastBusyKitchen();
        PizzaPacket   packet{type, size, ++_orderCounter};
        kitchen.send(packet);
        std::cout << "Order #" << packet.orderId << ": "
                  << PIZZA_NAMES.at(type) << " " << SIZE_NAMES.at(size)
                  << " → kitchen PID " << kitchen.pid() << "\n";
    }
}

void Reception::printStatus() const
{
    std::unique_lock lock(_kitchensMutex);
    if (_kitchens.empty()) {
        std::cout << "No kitchens active.\n";
        return;
    }
    for (std::size_t i = 0; i < _kitchens.size(); ++i) {
        std::cout << "Kitchen " << i + 1
                  << " (PID " << _kitchens[i]->pid() << ")"
                  << " pending=" << _kitchens[i]->pendingCount() << "\n";
    }
}

void Reception::reapDeadKitchens()
{
    _kitchens.erase(
        std::remove_if(_kitchens.begin(), _kitchens.end(),
            [](const std::unique_ptr<KitchenProxy> &k) { return !k->isAlive(); }),
        _kitchens.end());
}

void Reception::resultPollerLoop()
{
    std::ofstream log("plazza.log", std::ios::app);
    while (_running) {
        {
            std::unique_lock lock(_kitchensMutex);
            for (auto &kitchen : _kitchens) {
                PizzaPacket result{};
                while (kitchen->pollResult(result)) {
                    std::string msg = "Order #" + std::to_string(result.orderId)
                        + " (" + PIZZA_NAMES.at(result.type)
                        + " " + SIZE_NAMES.at(result.size) + ") is ready!\n";
                    std::cout << msg;
                    std::cout.flush();
                    log << msg;
                    log.flush();
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
