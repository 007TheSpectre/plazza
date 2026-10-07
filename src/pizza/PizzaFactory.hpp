#pragma once

#include "IPizza.hpp"
#include <memory>

/**
 * @brief Static factory that instantiates the correct IPizza subclass for a given type and size.
 *
 * Throws std::invalid_argument for unknown PizzaType values.
 */
class PizzaFactory {
public:
    /// Returns a heap-allocated pizza of the requested @p type and @p size.
    static std::unique_ptr<IPizza> create(PizzaType type, PizzaSize size);
};
