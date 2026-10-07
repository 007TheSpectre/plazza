#pragma once

#include "IIPC.hpp"
#include <string>

/// Whether to open the FIFO for reading or writing.
enum class PipeMode { ReadOnly, WriteOnly };

/**
 * @brief POSIX named pipe (FIFO) implementation of IIPC.
 *
 * Wire format: every message is preceded by a 4-byte little-endian length field.
 * Because the header + payload fit within PIPE_BUF (4096 B on Linux), each
 * send() is a single write() syscall — atomicity guaranteed by the kernel.
 *
 * Reception opens the result pipe with O_NONBLOCK first to avoid open-blocking
 * deadlock, then clears the flag. See KitchenProxy for the handshake sequence.
 */
class NamedPipe : public IIPC {
public:
    NamedPipe(const std::string &path, PipeMode mode);
    ~NamedPipe() override;

    /// Writes a 4-byte length header followed by @p data in one syscall.
    void                 send(const std::vector<uint8_t> &data) override;

    /// Blocks until a complete length-prefixed message is available, then returns it.
    std::vector<uint8_t> receive() override;

    /// Returns true if data is available within @p timeoutMs milliseconds (uses poll).
    bool                 hasData(int timeoutMs) const;

    /// Creates the underlying FIFO file at @p path (mkfifo). No-op if it already exists.
    static void create(const std::string &path);

    /// Unlinks the FIFO file at @p path.
    static void remove(const std::string &path);

    NamedPipe(const NamedPipe &) = delete;
    NamedPipe &operator=(const NamedPipe &) = delete;

private:
    std::string _path;
    int         _fd{-1};
};
