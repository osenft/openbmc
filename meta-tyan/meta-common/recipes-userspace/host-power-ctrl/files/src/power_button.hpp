#pragma once

#include "power_control_interface.hpp"

#include <gpioplus/handle.hpp>
#include <sdeventplus/clock.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace power_control
{

using Timer = sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic>;

// Class representing the host's power button (PWRBTN_N) as actionable by the
// BMC.
class PowerButton
{
  public:
    // Initialize the PowerButton class to work with the specified PowerControl.
    // The Event is used to schedule a timer to release the power button after
    // a specified duration. The Handle represents the GPIO pin to utilize.
    PowerButton(PowerControlInterface& power_control,
                sdeventplus::Event& event, gpioplus::Handle& handle);

    PowerButton(const PowerButton&) = delete;
    PowerButton& operator=(const PowerButton&) = delete;

    // Assert the power button for the specified duration.
    void Push(Timer::Duration duration);

    // Release the power button.
    void Release();

  private:
    void TimerHandler(Timer& timer);

    PowerControlInterface* power_control_;
    Timer timer_;
    gpioplus::Handle* handle_;
};

} // namespace power_control
