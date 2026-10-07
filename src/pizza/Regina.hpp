#pragma once

#include "IPizza.hpp"

/// Regina pizza: Dough + Tomato + Gruyere + Ham + Mushrooms. Base bake time 2.0 s.
class Regina : public IPizza {
public:
    explicit Regina(PizzaSize size);

    PizzaType               type()        const override;
    PizzaSize               size()        const override;
    double                  bakeTime()    const override;
    std::vector<Ingredient> ingredients() const override;
    std::string             name()        const override;

private:
    PizzaSize _size;
};
