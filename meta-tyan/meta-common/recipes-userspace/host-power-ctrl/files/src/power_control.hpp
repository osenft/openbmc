#pragma once

#include "power_button.hpp"
#include "power_control_interface.hpp"

#include <gpioplus/event.hpp>
#include <gpioplus/handle.hpp>
#include <sdbusplus/bus.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/source/io.hpp>
#include <string>
#include <xyz/openbmc_project/Chassis/Control/Power/server.hpp>

namespace power_control
{

using PowerInherit = sdbusplus::server::object_t<
    sdbusplus::xyz::openbmc_project::Chassis::Control::server::Power>;
namespace server = sdbusplus::xyz::openbmc_project::Chassis::Control::server;

// Class implementing the D-Bus Chassis.Control.Power interface.
// When the Chassis.Control.Power state property changes, the host is turned-on
// or off as requested. Uses the PWRBTN_N GPIO pin to execute the desired state
// change.
// When the PGOOD GPIO pin changes, the state property is updated. Further, the
// obmc-host-{start,stop} and obmc-chassis-power{on,off} SystemD targets are
// started to propagate the state change to the remaining system.
class PowerControl : public PowerInherit, public PowerControlInterface
{
  public:
    // Initialize PowerControl instance.
    // Arguments:
    // - bus: D-bus instance
    // - intf_name: D-bus interface name
    // - obj_path: D-bus path
    // - sd_event: event loop to register other triggers on
    // - pgood_event: GPIO event for PGOOD signal
    // - pwrbtn_handle: GPIO handle for PWRBTN_N
    PowerControl(sdbusplus::bus::bus& bus, const char* intf_name,
                 const char* obj_path, sdeventplus::Event& sd_event,
                 gpioplus::Event& pgood_event, gpioplus::Handle& pwrbtn_handle);

    PowerControl(const PowerControl&) = delete;
    PowerControl& operator=(const PowerControl&) = delete;

    // Implementation of PowerInherit methods
    bool forcePowerOff() override;
    int32_t state(int32_t value, bool skipSignal) override;

    // Implementation of PowerControlInterface methods
    PowerState GetState() const override;
    PowerState GetPGood() const override;

  private:
    // Apply the desired power state to the host, i.e. turns the host on or off
    // via the power button as requested.
    void ApplyPowerState(PowerState value);

    // Set the State property
    void SetState(PowerState value);

    // Set the PGood property
    void SetPGood(PowerState value);

    // Handle PGOOD GPIO pin event
    void PGoodEventHandler(sdeventplus::source::IO& source, int fd,
                           uint32_t revents);

    // Operate on PGOOD GPIO pin change
    void PGoodPinChanged(PowerState value);

    sdbusplus::bus::bus* bus_;
    gpioplus::Event* pgood_event_;
    PowerButton pwrbtn_;
    std::unique_ptr<sdeventplus::source::IO> pgood_io_event_;
};

} // namespace power_control
