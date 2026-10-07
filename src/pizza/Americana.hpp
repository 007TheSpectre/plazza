#pragma once

#include "IPizza.hpp"

/// Americana pizza: Dough + Tomato + Gruyere + Steak. Base bake time 2.0 s.
class Americana : public IPizza {
public:
    explicit Americana(PizzaSize size);

    PizzaType               type()        const override;
    PizzaSize               size()        const override;
    double                  bakeTime()    const override;
    std::vector<Ingredient> ingredients() const override;
    std::string             name()        const override;

private:
    PizzaSize _size;
};
