#include "Americana.hpp"

Americana::Americana(PizzaSize size) : _size(size) {}

PizzaType Americana::type() const { return PizzaType::Americana; }
PizzaSize Americana::size() const { return _size; }
double    Americana::bakeTime() const { return 2.0; }

std::vector<Ingredient> Americana::ingredients() const
{
    return {Ingredient::Dough, Ingredient::Tomato, Ingredient::Gruyere,
            Ingredient::Steak};
}

std::string Americana::name() const { return "Americana"; }
