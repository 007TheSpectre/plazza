#pragma once

/// Pizza type identifier. Values are powers of 2 for bitmask composability.
enum class PizzaType {
    Regina    = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia  = 8
};

/// Pizza size modifier. Values scale bake time proportionally.
enum class PizzaSize {
    S   = 1,
    M   = 2,
    L   = 4,
    XL  = 8,
    XXL = 16
};

/// Ingredients used in pizza recipes. Each pizza type requires a fixed subset.
enum class Ingredient {
    Dough,
    Tomato,
    Gruyere,
    Ham,
    Mushrooms,
    Steak,
    Eggplant,
    GoatCheese,
    ChiefLove
};
