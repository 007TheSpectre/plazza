#include "KitchenProxy.hpp"
#include <sys/wait.h>

KitchenProxy::KitchenProxy(pid_t pid,
                             const std::string &orderPipePath,
                             const std::string &resultPipePath,
                             std::size_t numCooks)
    : _pid(pid),
      _orderPipePath(orderPipePath),
      _resultPipePath(resultPipePath),
      _numCooks(numCooks)
{
    _resultPipe = std::make_unique<NamedPipe>(resultPipePath, PipeMode::ReadOnly);
    _orderPipe  = std::make_unique<NamedPipe>(orderPipePath, PipeMode::WriteOnly);
}

KitchenProxy::~KitchenProxy()
{
    _orderPipe.reset();
    _resultPipe.reset();
    NamedPipe::remove(_orderPipePath);
    NamedPipe::remove(_resultPipePath);
    ::waitpid(_pid, nullptr, 0);
}

void KitchenProxy::send(const PizzaPacket &packet)
{
    *_orderPipe << packet;
    ++_pending;
}

bool KitchenProxy::pollResult(PizzaPacket &out)
{
    if (!_resultPipe->hasData(0))
        return false;
    auto data = _resultPipe->receive();
    if (data.empty())
        return false;
    out = unpack(data);
    --_pending;
    return true;
}

bool KitchenProxy::isAlive() const
{
    int status = 0;
    return ::waitpid(_pid, &status, WNOHANG) == 0;
}

std::size_t KitchenProxy::pendingCount() const
{
    return _pending;
}

pid_t KitchenProxy::pid() const
{
    return _pid;
}
