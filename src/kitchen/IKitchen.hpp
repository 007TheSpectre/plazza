#pragma once

/// Abstract interface for a kitchen process. Allows Reception to call status() without depending on the concrete type.
class IKitchen {
public:
    virtual ~IKitchen() = default;

    /// Prints current cook queue depth and stock levels to stdout.
    virtual void status() const = 0;
};
