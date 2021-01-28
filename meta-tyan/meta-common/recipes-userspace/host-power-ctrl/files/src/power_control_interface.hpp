#pragma once

#include "power_state.hpp"

namespace power_control
{

class PowerControlInterface
{
  public:
    virtual ~PowerControlInterface() = default;

    // Get the "State" property as a PowerState
    virtual PowerState GetState() const = 0;

    // Get the "PGood" property as a PowerState
    virtual PowerState GetPGood() const = 0;

    PowerControlInterface(const PowerControlInterface&) = delete;
    PowerControlInterface& operator=(const PowerControlInterface&) = delete;

  protected:
    PowerControlInterface() = default;
};

} // namespace power_control
