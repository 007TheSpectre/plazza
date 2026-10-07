#pragma once

#include "IPizza.hpp"

/// Fantasia pizza: Dough + Tomato + Eggplant + GoatCheese + ChiefLove. Base bake time 4.0 s.
class Fantasia : public IPizza {
public:
    explicit Fantasia(PizzaSize size);

    PizzaType               type()        const override;
    PizzaSize               size()        const override;
    double                  bakeTime()    const override;
    std::vector<Ingredient> ingredients() const override;
    std::string             name()        const override;

private:
    PizzaSize _size;
};
