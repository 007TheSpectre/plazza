#pragma once

#include "IPizza.hpp"

/// Margarita pizza: Dough + Tomato + Gruyere. Base bake time 1.0 s.
class Margarita : public IPizza {
public:
    explicit Margarita(PizzaSize size);

    PizzaType               type()        const override;
    PizzaSize               size()        const override;
    double                  bakeTime()    const override;
    std::vector<Ingredient> ingredients() const override;
    std::string             name()        const override;

private:
    PizzaSize _size;
};
