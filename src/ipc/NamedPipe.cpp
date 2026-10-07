#include "NamedPipe.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

static constexpr std::size_t LENGTH_PREFIX_SIZE = sizeof(uint32_t);

NamedPipe::NamedPipe(const std::string &path, PipeMode mode) : _path(path)
{
    int flags = 0;
    if (mode == PipeMode::WriteOnly) {
        flags = O_WRONLY;
    } else {
        flags = O_RDONLY | O_NONBLOCK;
    }

    _fd = ::open(path.c_str(), flags);
    if (_fd == -1)
        throw std::runtime_error("NamedPipe open failed: " + path + " — " + std::strerror(errno));

    if (mode == PipeMode::ReadOnly) {
        int currentFlags = ::fcntl(_fd, F_GETFL);
        ::fcntl(_fd, F_SETFL, currentFlags & ~O_NONBLOCK);
    }
}

NamedPipe::~NamedPipe()
{
    if (_fd != -1)
        ::close(_fd);
}

void NamedPipe::send(const std::vector<uint8_t> &data)
{
    uint32_t len = static_cast<uint32_t>(data.size());
    std::vector<uint8_t> buf(LENGTH_PREFIX_SIZE + data.size());
    std::memcpy(buf.data(), &len, LENGTH_PREFIX_SIZE);
    std::memcpy(buf.data() + LENGTH_PREFIX_SIZE, data.data(), data.size());
    ssize_t written = ::write(_fd, buf.data(), buf.size());
    if (written < 0 && errno == EPIPE)
        return;
    if (written != static_cast<ssize_t>(buf.size()))
        throw std::runtime_error("NamedPipe write failed");
}

std::vector<uint8_t> NamedPipe::receive()
{
    uint32_t len = 0;
    ssize_t  r   = ::read(_fd, &len, LENGTH_PREFIX_SIZE);
    if (r == 0)
        return {};
    if (r != static_cast<ssize_t>(LENGTH_PREFIX_SIZE))
        throw std::runtime_error("NamedPipe read length failed");
    std::vector<uint8_t> buf(len);
    if (::read(_fd, buf.data(), len) != static_cast<ssize_t>(len))
        throw std::runtime_error("NamedPipe read data failed");
    return buf;
}

bool NamedPipe::hasData(int timeoutMs) const
{
    struct pollfd pfd{};
    pfd.fd     = _fd;
    pfd.events = POLLIN;
    return ::poll(&pfd, 1, timeoutMs) > 0 && (pfd.revents & POLLIN);
}

void NamedPipe::create(const std::string &path)
{
    if (::mkfifo(path.c_str(), 0600) == -1 && errno != EEXIST)
        throw std::runtime_error("NamedPipe mkfifo failed: " + path);
}

void NamedPipe::remove(const std::string &path)
{
    ::unlink(path.c_str());
}
