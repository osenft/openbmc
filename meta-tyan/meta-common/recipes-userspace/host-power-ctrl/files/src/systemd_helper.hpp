#pragma once

#include <sdbusplus/bus.hpp>

namespace power_control
{

// Helper to start the specified SystemD unit.
void SystemdStartUnit(sdbusplus::bus::bus& bus, const char* unit);

} // namespace power_control
