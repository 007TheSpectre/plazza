#include "Fantasia.hpp"

Fantasia::Fantasia(PizzaSize size) : _size(size) {}

PizzaType Fantasia::type() const { return PizzaType::Fantasia; }
PizzaSize Fantasia::size() const { return _size; }
double    Fantasia::bakeTime() const { return 4.0; }

std::vector<Ingredient> Fantasia::ingredients() const
{
    return {Ingredient::Dough, Ingredient::Tomato, Ingredient::Eggplant,
            Ingredient::GoatCheese, Ingredient::ChiefLove};
}

std::string Fantasia::name() const { return "Fantasia"; }
