#include <criterion/criterion.h>
#include "pizza/PizzaFactory.hpp"
#include "pizza/PizzaTypes.hpp"

Test(PizzaFactoryTest, CreatesMargarita)
{
    auto pizza = PizzaFactory::create(PizzaType::Margarita, PizzaSize::S);
    cr_assert(pizza != nullptr);
    cr_assert(pizza->type() == PizzaType::Margarita);
    cr_assert(pizza->name() == std::string("Margarita"));
}

Test(PizzaFactoryTest, CreatesRegina)
{
    auto pizza = PizzaFactory::create(PizzaType::Regina, PizzaSize::M);
    cr_assert(pizza != nullptr);
    cr_assert(pizza->type() == PizzaType::Regina);
    cr_assert(pizza->name() == std::string("Regina"));
}

Test(PizzaFactoryTest, CreatesAmericana)
{
    auto pizza = PizzaFactory::create(PizzaType::Americana, PizzaSize::L);
    cr_assert(pizza != nullptr);
    cr_assert(pizza->type() == PizzaType::Americana);
    cr_assert(pizza->name() == std::string("Americana"));
}

Test(PizzaFactoryTest, CreatesFantasia)
{
    auto pizza = PizzaFactory::create(PizzaType::Fantasia, PizzaSize::XL);
    cr_assert(pizza != nullptr);
    cr_assert(pizza->type() == PizzaType::Fantasia);
    cr_assert(pizza->name() == std::string("Fantasia"));
}

Test(PizzaFactoryTest, PreservesRequestedSize)
{
    for (auto size : {PizzaSize::S, PizzaSize::M, PizzaSize::L, PizzaSize::XL, PizzaSize::XXL}) {
        auto pizza = PizzaFactory::create(PizzaType::Margarita, size);
        cr_assert(pizza->size() == size);
    }
}

Test(PizzaFactoryTest, AllTypesHaveNonEmptyIngredients)
{
    for (auto type : {PizzaType::Regina, PizzaType::Margarita, PizzaType::Americana, PizzaType::Fantasia}) {
        auto pizza = PizzaFactory::create(type, PizzaSize::S);
        cr_assert_not(pizza->ingredients().empty());
    }
}

Test(PizzaFactoryTest, AllTypesHavePositiveBakeTime)
{
    for (auto type : {PizzaType::Regina, PizzaType::Margarita, PizzaType::Americana, PizzaType::Fantasia}) {
        auto pizza = PizzaFactory::create(type, PizzaSize::S);
        cr_assert_gt(pizza->bakeTime(), 0.0);
    }
}

Test(PizzaFactoryTest, ThrowsOnInvalidType)
{
    cr_assert_throw(
        PizzaFactory::create(static_cast<PizzaType>(99), PizzaSize::S),
        std::invalid_argument);
}
