#include "power_state.hpp"

namespace power_control
{

PowerState ToPowerState(bool value)
{
    if (value)
    {
        return PowerState::ON;
    }
    else
    {
        return PowerState::OFF;
    }
}

PowerState ToPowerState(int value)
{
    return ToPowerState(value != 0);
}

} // namespace power_control
