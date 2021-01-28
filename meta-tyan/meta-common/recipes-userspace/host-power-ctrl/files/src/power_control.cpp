#include "power_control.hpp"

#include "systemd_helper.hpp"

#include <phosphor-logging/elog-errors.hpp>

namespace power_control
{

constexpr auto HOST_START_UNIT = "obmc-host-start@0.target";
constexpr auto HOST_STOP_UNIT = "obmc-host-stop@0.target";
constexpr auto CHASSIS_POWER_ON_UNIT = "obmc-chassis-poweron@0.target";
constexpr auto CHASSIS_POWER_OFF_UNIT = "obmc-chassis-poweroff@0.target";

constexpr auto POWER_ON_PULSE_TIME = std::chrono::seconds(1);
constexpr auto POWER_OFF_WAIT_TIME = std::chrono::seconds(10);

PowerControl::PowerControl(sdbusplus::bus::bus& bus, const char* intf_name,
                           const char* obj_path, sdeventplus::Event& sd_event,
                           gpioplus::Event& pgood_event,
                           gpioplus::Handle& pwrbtn_handle) :
    PowerInherit(bus, obj_path, true),
    bus_(&bus), pgood_event_(&pgood_event),
    pwrbtn_(*this, sd_event, pwrbtn_handle)
{
    // Make sure PGoodEventHandler does not block on event read
    pgood_event_->getFd().setBlocking(false);

    pgood_io_event_ = std::make_unique<sdeventplus::source::IO>(
        sd_event, *(pgood_event_->getFd()), EPOLLIN,
        std::bind(std::mem_fn(&PowerControl::PGoodEventHandler),
                  this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));

    // Apply initial PGood pin state
    PGoodPinChanged(ToPowerState(pgood_event_->getValue() != 0));
    emit_object_added();
}

bool PowerControl::forcePowerOff()
{
    ApplyPowerState(PowerState::OFF);
    return true;
}

int32_t PowerControl::state(int32_t value, bool skipSignal)
{
    phosphor::logging::log<phosphor::logging::level::DEBUG>("state()");
    auto val = PowerInherit::state(value, skipSignal);

    ApplyPowerState(ToPowerState(value));

    return val;
}

void PowerControl::ApplyPowerState(PowerState value)
{
    phosphor::logging::log<phosphor::logging::level::DEBUG>(
        "ApplyPowerState", phosphor::logging::entry("VALUE=%d", value));

    // Check if value is different from pgood
    if (value == GetPGood())
    {
        phosphor::logging::log<phosphor::logging::level::INFO>(
            "same powerstate", phosphor::logging::entry("VALUE=%d", value));
        return;
    }

    switch (value)
    {
        case PowerState::ON:
            pwrbtn_.Push(POWER_ON_PULSE_TIME);
            break;
        case PowerState::OFF:
            pwrbtn_.Push(POWER_OFF_WAIT_TIME);
            break;
    }
}

void PowerControl::SetState(PowerState value)
{
    switch (value)
    {
        case PowerState::OFF:
            server::Power::state(0);
            break;
        case PowerState::ON:
            server::Power::state(1);
            break;
    }
}

PowerState PowerControl::GetState() const
{
    return ToPowerState(server::Power::state());
}

PowerState PowerControl::GetPGood() const
{
    return ToPowerState(server::Power::pGood());
}

void PowerControl::SetPGood(PowerState value)
{
    switch (value)
    {
        case PowerState::OFF:
            server::Power::pGood(false);
            break;
        case PowerState::ON:
            server::Power::pGood(true);
            break;
    }
}

void PowerControl::PGoodEventHandler(sdeventplus::source::IO& source, int fd,
                                     uint32_t revents)
{
    phosphor::logging::log<phosphor::logging::level::DEBUG>(
        "PGoodEventHandler");

    auto data = pgood_event_->read();
    if (!data)
    {
        phosphor::logging::log<phosphor::logging::level::ERR>(
            "no data from pgood event");
        return;
    }

    PGoodPinChanged(ToPowerState(data->id == GPIOEVENT_EVENT_RISING_EDGE));
}

void PowerControl::PGoodPinChanged(PowerState value)
{
    if (value == PowerState::OFF)
    {
        // We've just gone OFF. Make sure the power button is no longer pressed.
        pwrbtn_.Release();
    }

    // Update "pGood" first as otherwise the change of "state" will attempt
    // another power-on / power-off.
    SetPGood(value);
    SetState(value);
    switch (value)
    {
        case PowerState::OFF:
            SystemdStartUnit(*bus_, HOST_STOP_UNIT);
            SystemdStartUnit(*bus_, CHASSIS_POWER_OFF_UNIT);
            break;
        case PowerState::ON:
            SystemdStartUnit(*bus_, HOST_START_UNIT);
            SystemdStartUnit(*bus_, CHASSIS_POWER_ON_UNIT);
            break;
    }
}

} // namespace power_control
