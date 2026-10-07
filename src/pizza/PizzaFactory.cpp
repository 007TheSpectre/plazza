#include "PizzaFactory.hpp"
#include "Americana.hpp"
#include "Fantasia.hpp"
#include "Margarita.hpp"
#include "Regina.hpp"
#include <stdexcept>

std::unique_ptr<IPizza> PizzaFactory::create(PizzaType type, PizzaSize size)
{
    switch (type) {
    case PizzaType::Margarita: return std::make_unique<Margarita>(size);
    case PizzaType::Regina:    return std::make_unique<Regina>(size);
    case PizzaType::Americana: return std::make_unique<Americana>(size);
    case PizzaType::Fantasia:  return std::make_unique<Fantasia>(size);
    }
    throw std::invalid_argument("Unknown pizza type");
}
