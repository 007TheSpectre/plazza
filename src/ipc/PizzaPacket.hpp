#pragma once

#include "../pizza/PizzaTypes.hpp"
#include "IIPC.hpp"
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

/// Wire format for a single pizza order or result sent through a NamedPipe.
struct PizzaPacket {
    PizzaType type;     ///< Pizza variety.
    PizzaSize size;     ///< Ordered size.
    uint32_t  orderId;  ///< Monotonically increasing order number assigned by Reception.
};

/// Serialises @p p into a raw byte buffer.
inline std::vector<uint8_t> pack(const PizzaPacket &p)
{
    std::vector<uint8_t> buf(sizeof(PizzaPacket));
    std::memcpy(buf.data(), &p, sizeof(PizzaPacket));
    return buf;
}

/// Deserialises a PizzaPacket from @p buf; throws if the buffer is too small.
inline PizzaPacket unpack(const std::vector<uint8_t> &buf)
{
    if (buf.size() < sizeof(PizzaPacket))
        throw std::runtime_error("PizzaPacket: buffer too small");
    PizzaPacket p{};
    std::memcpy(&p, buf.data(), sizeof(PizzaPacket));
    return p;
}

/// Sends @p packet over @p ipc using the pack/unpack wire format.
inline IIPC &operator<<(IIPC &ipc, const PizzaPacket &packet)
{
    ipc.send(pack(packet));
    return ipc;
}

/// Receives one packet from @p ipc into @p packet.
inline IIPC &operator>>(IIPC &ipc, PizzaPacket &packet)
{
    packet = unpack(ipc.receive());
    return ipc;
}
