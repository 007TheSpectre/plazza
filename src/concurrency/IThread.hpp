#pragma once

/// Abstract thread interface. Implemented by the Thread template.
class IThread {
public:
    virtual ~IThread() = default;

    /// Starts the thread. Must not be called more than once.
    virtual void start() = 0;

    /// Blocks until the thread finishes.
    virtual void join() = 0;

    /// Returns true while the thread callable is executing.
    virtual bool isRunning() const = 0;
};
