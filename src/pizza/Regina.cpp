#include "Regina.hpp"

Regina::Regina(PizzaSize size) : _size(size) {}

PizzaType Regina::type() const { return PizzaType::Regina; }
PizzaSize Regina::size() const { return _size; }
double    Regina::bakeTime() const { return 2.0; }

std::vector<Ingredient> Regina::ingredients() const
{
    return {Ingredient::Dough, Ingredient::Tomato, Ingredient::Gruyere,
            Ingredient::Ham, Ingredient::Mushrooms};
}

std::string Regina::name() const { return "Regina"; }
