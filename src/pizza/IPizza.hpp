#pragma once

#include "PizzaTypes.hpp"
#include <string>
#include <vector>

/// Abstract interface every concrete pizza type must implement.
class IPizza {
public:
    virtual ~IPizza() = default;

    /// Returns the pizza variety (Regina, Margarita, …).
    virtual PizzaType                type()        const = 0;

    /// Returns the ordered size (S … XXL).
    virtual PizzaSize                size()        const = 0;

    /// Returns the base bake time in seconds before the time multiplier is applied.
    virtual double                   bakeTime()    const = 0;

    /// Returns the list of ingredients consumed from Stock during cooking.
    virtual std::vector<Ingredient>  ingredients() const = 0;

    /// Returns a human-readable name for display and logging.
    virtual std::string              name()        const = 0;
};
