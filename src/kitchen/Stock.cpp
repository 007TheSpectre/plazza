#include "Stock.hpp"
#include <iostream>

static const std::vector<Ingredient> ALL_INGREDIENTS = {
    Ingredient::Dough,     Ingredient::Tomato,    Ingredient::Gruyere,
    Ingredient::Ham,       Ingredient::Mushrooms, Ingredient::Steak,
    Ingredient::Eggplant,  Ingredient::GoatCheese,Ingredient::ChiefLove
};

static const std::map<Ingredient, std::string> INGREDIENT_NAMES = {
    {Ingredient::Dough,      "Dough"},
    {Ingredient::Tomato,     "Tomato"},
    {Ingredient::Gruyere,    "Gruyere"},
    {Ingredient::Ham,        "Ham"},
    {Ingredient::Mushrooms,  "Mushrooms"},
    {Ingredient::Steak,      "Steak"},
    {Ingredient::Eggplant,   "Eggplant"},
    {Ingredient::GoatCheese, "GoatCheese"},
    {Ingredient::ChiefLove,  "ChiefLove"}
};

Stock::Stock(std::chrono::milliseconds restockInterval)
    : _restockInterval(restockInterval)
{
    for (auto ingredient : ALL_INGREDIENTS)
        _items[ingredient] = 5;
    _restockThread = std::thread([this] { replenishLoop(); });
}

Stock::~Stock()
{
    {
        std::unique_lock lock(_cvMutex);
        _running = false;
        _cv.notify_one();
    }
    if (_restockThread.joinable())
        _restockThread.join();
}

bool Stock::consume(const std::vector<Ingredient> &ingredients)
{
    std::unique_lock lock(_mutex);
    for (auto ing : ingredients)
        if (_items.at(ing) <= 0)
            return false;
    for (auto ing : ingredients)
        --_items.at(ing);
    return true;
}

void Stock::status() const
{
    std::unique_lock lock(_mutex);
    for (auto &[ing, count] : _items)
        std::cout << "    " << INGREDIENT_NAMES.at(ing) << ": " << count << "\n";
}

void Stock::replenishLoop()
{
    while (true) {
        std::unique_lock cvLock(_cvMutex);
        if (_cv.wait_for(cvLock, _restockInterval, [this] { return !_running.load(); }))
            return;
        cvLock.unlock();
        std::unique_lock lock(_mutex);
        for (auto &[ing, count] : _items)
            ++count;
    }
}
