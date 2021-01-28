#include "power_button.hpp"

#include <phosphor-logging/elog-errors.hpp>

namespace power_control
{

PowerButton::PowerButton(PowerControlInterface& power_control,
                         sdeventplus::Event& event, gpioplus::Handle& handle) :
    power_control_(&power_control),
    timer_(event, std::bind(std::mem_fn(&PowerButton::TimerHandler), this,
                            std::placeholders::_1)),
    handle_(&handle)
{
}

void PowerButton::TimerHandler(Timer& timer)
{
    phosphor::logging::log<phosphor::logging::level::DEBUG>(
        "PowerButton::TimerHandler");
    Release();
    if (power_control_->GetState() == PowerState::OFF &&
        power_control_->GetPGood() == PowerState::ON)
    {
        phosphor::logging::log<phosphor::logging::level::WARNING>(
            "pgood still ON");
    }
}

void PowerButton::Push(Timer::Duration duration)
{
    phosphor::logging::log<phosphor::logging::level::DEBUG>(
        "PowerButton::Push");
    handle_->setValues({1});
    timer_.restartOnce(duration);
}

void PowerButton::Release()
{
    phosphor::logging::log<phosphor::logging::level::DEBUG>(
        "PowerButton::Release");
    timer_.setEnabled(false);
    handle_->setValues({0});
}

} // namespace power_control
