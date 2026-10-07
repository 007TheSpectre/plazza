#include <criterion/criterion.h>
#include "ipc/PizzaPacket.hpp"
#include <limits>

Test(PizzaPacketTest, RoundTripPreservesAllFields)
{
    PizzaPacket original{PizzaType::Regina, PizzaSize::XL, 99};
    PizzaPacket result = unpack(pack(original));
    cr_assert(result.type == original.type);
    cr_assert(result.size == original.size);
    cr_assert_eq(result.orderId, original.orderId);
}

Test(PizzaPacketTest, AllTypesRoundTrip)
{
    for (auto type : {PizzaType::Regina, PizzaType::Margarita, PizzaType::Americana, PizzaType::Fantasia}) {
        PizzaPacket p{type, PizzaSize::S, 0};
        cr_assert(unpack(pack(p)).type == type);
    }
}

Test(PizzaPacketTest, AllSizesRoundTrip)
{
    for (auto size : {PizzaSize::S, PizzaSize::M, PizzaSize::L, PizzaSize::XL, PizzaSize::XXL}) {
        PizzaPacket p{PizzaType::Margarita, size, 0};
        cr_assert(unpack(pack(p)).size == size);
    }
}

Test(PizzaPacketTest, OrderIdBoundaries)
{
    for (uint32_t id : {0u, 1u, std::numeric_limits<uint32_t>::max()}) {
        PizzaPacket p{PizzaType::Americana, PizzaSize::L, id};
        cr_assert_eq(unpack(pack(p)).orderId, id);
    }
}

Test(PizzaPacketTest, UnpackThrowsOnEmptyBuffer)
{
    cr_assert_throw(unpack({}), std::runtime_error);
}

Test(PizzaPacketTest, UnpackThrowsOnUndersizedBuffer)
{
    std::vector<uint8_t> small(sizeof(PizzaPacket) - 1, 0);
    cr_assert_throw(unpack(small), std::runtime_error);
}

Test(PizzaPacketTest, UnpackAcceptsExactSizeBufferAndPreservesValues)
{
    PizzaPacket original{PizzaType::Fantasia, PizzaSize::XXL, 7};
    PizzaPacket result = unpack(pack(original));
    cr_assert(result.type == PizzaType::Fantasia);
    cr_assert(result.size == PizzaSize::XXL);
    cr_assert_eq(result.orderId, 7u);
}
