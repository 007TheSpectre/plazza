#pragma once

#include "../pizza/PizzaTypes.hpp"
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Stateless order-line parser.
 *
 * Accepts lines in the format: `<type> <size> x<N>[;<type> <size> x<N>...]`
 * and returns a flat list of (PizzaType, PizzaSize) pairs expanded by count.
 * Throws std::runtime_error on any syntax or unknown-token error.
 */
class Parser {
public:
    /// Flat list of individual pizzas extracted from one order line.
    using Order = std::vector<std::pair<PizzaType, PizzaSize>>;

    /// Parses @p line and returns the expanded order. Throws on malformed input.
    static Order parse(const std::string &line);

private:
    static PizzaType parseType(const std::string &token);
    static PizzaSize parseSize(const std::string &token);
    static int       parseCount(const std::string &token);
};
