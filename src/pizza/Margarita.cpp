#include "Margarita.hpp"

Margarita::Margarita(PizzaSize size) : _size(size) {}

PizzaType Margarita::type() const { return PizzaType::Margarita; }
PizzaSize Margarita::size() const { return _size; }
double    Margarita::bakeTime() const { return 1.0; }

std::vector<Ingredient> Margarita::ingredients() const
{
    return {Ingredient::Dough, Ingredient::Tomato, Ingredient::Gruyere};
}

std::string Margarita::name() const { return "Margarita"; }
