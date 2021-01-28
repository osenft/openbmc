#pragma once

namespace power_control
{

// Known power states
enum class PowerState
{
    OFF,
    ON
};

PowerState ToPowerState(bool value);
PowerState ToPowerState(int value);

} // namespace power_control
