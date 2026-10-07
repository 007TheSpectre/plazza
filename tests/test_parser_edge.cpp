#include <criterion/criterion.h>
#include "reception/Parser.hpp"

Test(ParserEdgeTest, EmptyStringReturnsEmptyOrder)
{
    auto order = Parser::parse("");
    cr_assert(order.empty());
}

Test(ParserEdgeTest, ThrowsOnWhitespaceOnly)
{
    cr_assert_throw(Parser::parse("   "), std::runtime_error);
}

Test(ParserEdgeTest, ParsesLargeCount)
{
    auto order = Parser::parse("Margarita S x100");
    cr_assert_eq(order.size(), 100u);
    for (auto &[type, size] : order) {
        cr_assert(type == PizzaType::Margarita);
        cr_assert(size == PizzaSize::S);
    }
}

Test(ParserEdgeTest, LeadingTrailingWhitespaceInSegmentSucceeds)
{
    auto order = Parser::parse("  Margarita S x1  ");
    cr_assert_eq(order.size(), 1u);
    cr_assert(order[0].first == PizzaType::Margarita);
    cr_assert(order[0].second == PizzaSize::S);
}

Test(ParserEdgeTest, TrailingSemicolonIsIgnored)
{
    auto order = Parser::parse("Margarita S x1;");
    cr_assert_eq(order.size(), 1u);
    cr_assert(order[0].first == PizzaType::Margarita);
}

Test(ParserEdgeTest, ThrowsOnLeadingSemicolon)
{
    cr_assert_throw(Parser::parse(";Margarita S x1"), std::runtime_error);
}

Test(ParserEdgeTest, ParsesAllPizzaSizesInLargeOrders)
{
    auto order = Parser::parse("Margarita S x2;Regina M x3;Americana L x1;Fantasia XL x1;Regina XXL x1");
    cr_assert_eq(order.size(), 8u);
    cr_assert(order[0].first == PizzaType::Margarita);  cr_assert(order[0].second == PizzaSize::S);
    cr_assert(order[1].first == PizzaType::Margarita);  cr_assert(order[1].second == PizzaSize::S);
    cr_assert(order[2].first == PizzaType::Regina);     cr_assert(order[2].second == PizzaSize::M);
    cr_assert(order[5].first == PizzaType::Americana);  cr_assert(order[5].second == PizzaSize::L);
    cr_assert(order[6].first == PizzaType::Fantasia);   cr_assert(order[6].second == PizzaSize::XL);
    cr_assert(order[7].first == PizzaType::Regina);     cr_assert(order[7].second == PizzaSize::XXL);
}
