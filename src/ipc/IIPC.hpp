#pragma once

#include <cstdint>
#include <vector>

/// Abstract IPC channel. Concrete implementations (e.g. NamedPipe) provide the transport.
class IIPC {
public:
    virtual ~IIPC() = default;

    /// Writes @p data atomically to the channel.
    virtual void                   send(const std::vector<uint8_t> &data) = 0;

    /// Blocks until a complete message is available, then returns it.
    virtual std::vector<uint8_t>   receive() = 0;
};
