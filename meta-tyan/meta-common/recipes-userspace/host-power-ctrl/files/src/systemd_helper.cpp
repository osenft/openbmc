#include "systemd_helper.hpp"

namespace power_control
{

constexpr auto SYSTEMD_BUSNAME = "org.freedesktop.systemd1";
constexpr auto SYSTEMD_PATH = "/org/freedesktop/systemd1";
constexpr auto SYSTEMD_INTERFACE = "org.freedesktop.systemd1.Manager";

// TODO(b/132970709): implement helpers in sdbusplus to start D-Bus unit?
void SystemdStartUnit(sdbusplus::bus::bus& bus, const char* unit)
{
    auto method = bus.new_method_call(SYSTEMD_BUSNAME, SYSTEMD_PATH,
                                      SYSTEMD_INTERFACE, "StartUnit");
    method.append(unit, "replace");
    bus.call(method);
}

} // namespace power_control
