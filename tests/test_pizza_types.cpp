#include <criterion/criterion.h>
#include "pizza/Americana.hpp"
#include "pizza/Fantasia.hpp"
#include "pizza/Margarita.hpp"
#include "pizza/Regina.hpp"
#include "pizza/PizzaTypes.hpp"
#include <algorithm>

static bool hasIngredient(const std::vector<Ingredient> &list, Ingredient ing)
{
    return std::find(list.begin(), list.end(), ing) != list.end();
}

Test(MargaritaTest, TypeAndName)
{
    Margarita m(PizzaSize::M);
    cr_assert(m.type() == PizzaType::Margarita);
    cr_assert(m.name() == std::string("Margarita"));
}

Test(MargaritaTest, SizeIsPreserved)
{
    for (auto sz : {PizzaSize::S, PizzaSize::M, PizzaSize::L, PizzaSize::XL, PizzaSize::XXL})
        cr_assert(Margarita(sz).size() == sz);
}

Test(MargaritaTest, BakeTime)
{
    cr_assert_float_eq(Margarita(PizzaSize::S).bakeTime(), 1.0, 1e-9);
}

Test(MargaritaTest, Ingredients)
{
    auto ing = Margarita(PizzaSize::S).ingredients();
    cr_assert_eq(ing.size(), 3u);
    cr_assert(hasIngredient(ing, Ingredient::Dough));
    cr_assert(hasIngredient(ing, Ingredient::Tomato));
    cr_assert(hasIngredient(ing, Ingredient::Gruyere));
}

Test(ReginaTest, TypeAndName)
{
    Regina r(PizzaSize::L);
    cr_assert(r.type() == PizzaType::Regina);
    cr_assert(r.name() == std::string("Regina"));
}

Test(ReginaTest, BakeTime)
{
    cr_assert_float_eq(Regina(PizzaSize::S).bakeTime(), 2.0, 1e-9);
}

Test(ReginaTest, Ingredients)
{
    auto ing = Regina(PizzaSize::S).ingredients();
    cr_assert_eq(ing.size(), 5u);
    cr_assert(hasIngredient(ing, Ingredient::Dough));
    cr_assert(hasIngredient(ing, Ingredient::Tomato));
    cr_assert(hasIngredient(ing, Ingredient::Gruyere));
    cr_assert(hasIngredient(ing, Ingredient::Ham));
    cr_assert(hasIngredient(ing, Ingredient::Mushrooms));
}

Test(AmericanaTest, TypeAndName)
{
    Americana a(PizzaSize::XL);
    cr_assert(a.type() == PizzaType::Americana);
    cr_assert(a.name() == std::string("Americana"));
}

Test(AmericanaTest, BakeTime)
{
    cr_assert_float_eq(Americana(PizzaSize::S).bakeTime(), 2.0, 1e-9);
}

Test(AmericanaTest, Ingredients)
{
    auto ing = Americana(PizzaSize::S).ingredients();
    cr_assert_eq(ing.size(), 4u);
    cr_assert(hasIngredient(ing, Ingredient::Dough));
    cr_assert(hasIngredient(ing, Ingredient::Tomato));
    cr_assert(hasIngredient(ing, Ingredient::Gruyere));
    cr_assert(hasIngredient(ing, Ingredient::Steak));
}

Test(FantasiaTest, TypeAndName)
{
    Fantasia f(PizzaSize::XXL);
    cr_assert(f.type() == PizzaType::Fantasia);
    cr_assert(f.name() == std::string("Fantasia"));
}

Test(FantasiaTest, BakeTime)
{
    cr_assert_float_eq(Fantasia(PizzaSize::S).bakeTime(), 4.0, 1e-9);
}

Test(FantasiaTest, Ingredients)
{
    auto ing = Fantasia(PizzaSize::S).ingredients();
    cr_assert_eq(ing.size(), 5u);
    cr_assert(hasIngredient(ing, Ingredient::Dough));
    cr_assert(hasIngredient(ing, Ingredient::Tomato));
    cr_assert(hasIngredient(ing, Ingredient::Eggplant));
    cr_assert(hasIngredient(ing, Ingredient::GoatCheese));
    cr_assert(hasIngredient(ing, Ingredient::ChiefLove));
}
