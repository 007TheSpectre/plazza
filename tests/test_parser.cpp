#include <criterion/criterion.h>
#include "reception/Parser.hpp"

Test(ParserTest, ParsesSingleOrder)
{
    auto order = Parser::parse("Margarita S x1");
    cr_assert_eq(order.size(), 1u);
    cr_assert(order[0].first == PizzaType::Margarita);
    cr_assert(order[0].second == PizzaSize::S);
}

Test(ParserTest, CaseInsensitiveTypeAndSize)
{
    auto order = Parser::parse("MARGARITA s x1");
    cr_assert_eq(order.size(), 1u);
    cr_assert(order[0].first == PizzaType::Margarita);
    cr_assert(order[0].second == PizzaSize::S);
}

Test(ParserTest, CountExpandsEntries)
{
    auto order = Parser::parse("Fantasia XL x3");
    cr_assert_eq(order.size(), 3u);
    for (auto &[type, size] : order) {
        cr_assert(type == PizzaType::Fantasia);
        cr_assert(size == PizzaSize::XL);
    }
}

Test(ParserTest, ParsesMultipleSegments)
{
    auto order = Parser::parse("Margarita S x2;Regina M x1");
    cr_assert_eq(order.size(), 3u);
    cr_assert(order[0].first == PizzaType::Margarita);
    cr_assert(order[2].first == PizzaType::Regina);
    cr_assert(order[2].second == PizzaSize::M);
}

Test(ParserTest, ParsesAllSizes)
{
    for (auto sz : {"S", "M", "L", "XL", "XXL"})
        cr_assert_no_throw(Parser::parse(std::string("Margarita ") + sz + " x1"));
}

Test(ParserTest, ParsesAllTypes)
{
    for (auto t : {"Regina", "Margarita", "Americana", "Fantasia"})
        cr_assert_no_throw(Parser::parse(std::string(t) + " S x1"));
}

Test(ParserTest, ThrowsOnUnknownType)
{
    cr_assert_throw(Parser::parse("Calzone S x1"), std::runtime_error);
}

Test(ParserTest, ThrowsOnUnknownSize)
{
    cr_assert_throw(Parser::parse("Margarita XXXL x1"), std::runtime_error);
}

Test(ParserTest, ThrowsOnMissingXPrefix)
{
    cr_assert_throw(Parser::parse("Margarita S 1"), std::runtime_error);
}

Test(ParserTest, ThrowsOnZeroCount)
{
    cr_assert_throw(Parser::parse("Margarita S x0"), std::runtime_error);
}

Test(ParserTest, ThrowsOnNegativeCount)
{
    cr_assert_throw(Parser::parse("Margarita S x-1"), std::runtime_error);
}

Test(ParserTest, ThrowsOnMalformedSegment)
{
    cr_assert_throw(Parser::parse("Margarita"), std::runtime_error);
}

Test(ParserTest, ThrowsOnExtraToken)
{
    cr_assert_throw(Parser::parse("Margarita S x1 extra"), std::runtime_error);
}

Test(ParserTest, MultiSegmentOrderCount)
{
    auto order = Parser::parse("Americana XL x2;Fantasia M x3;Regina S x1");
    cr_assert_eq(order.size(), 6u);
}

Test(ParserTest, ThrowsOnEmptyCountDigits)
{
    cr_assert_throw(Parser::parse("Margarita S x"), std::runtime_error);
}
