#include <criterion/criterion.h>
#include "kitchen/Stock.hpp"
#include <chrono>
#include <thread>

static Stock makeStock()
{
    return Stock(std::chrono::hours(24));
}

Test(StockTest, ConsumeSucceedsWithInitialStock)
{
    auto stock = makeStock();
    cr_assert(stock.consume({Ingredient::Dough, Ingredient::Tomato}));
}

Test(StockTest, ConsumeReducesAvailableItems)
{
    auto stock = makeStock();
    for (int i = 0; i < 5; ++i)
        cr_assert(stock.consume({Ingredient::Dough}));
}

Test(StockTest, ConsumeFailsWhenStockExhausted)
{
    auto stock = makeStock();
    for (int i = 0; i < 5; ++i)
        stock.consume({Ingredient::Dough});
    cr_assert_not(stock.consume({Ingredient::Dough}));
}

Test(StockTest, ConsumeChecksAllIngredientsBeforeDeducting)
{
    auto stock = makeStock();
    for (int i = 0; i < 5; ++i)
        stock.consume({Ingredient::Dough});
    cr_assert_not(stock.consume({Ingredient::Dough, Ingredient::Tomato}));
    cr_assert(stock.consume({Ingredient::Tomato}));
}

Test(StockTest, IndependentIngredientsDoNotAffectEachOther)
{
    auto stock = makeStock();
    for (int i = 0; i < 5; ++i)
        stock.consume({Ingredient::Ham});
    cr_assert(stock.consume({Ingredient::Steak}));
    cr_assert(stock.consume({Ingredient::Mushrooms}));
}

Test(StockTest, RestockReplenishesIngredients)
{
    Stock stock(std::chrono::milliseconds(50));
    for (int i = 0; i < 5; ++i)
        stock.consume({Ingredient::Dough});
    cr_assert_not(stock.consume({Ingredient::Dough}));
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    cr_assert(stock.consume({Ingredient::Dough}));
}
