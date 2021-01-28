/*
 * Polls a GPIO (PGOOD) and asserts
 * - obmc-host-start@0, obmc-chassis-poweron@0 to signal when the GPIO has
 *   rising edge
 * - obmc-host-stop@0, obmc-chassis-poweroff@0 to signal when the GPIO has
 *   falling edge
 * Implement D-bus Chassis.Control.Power interface and operate host power status
 * as requested via the status property.
 */

#include "power_control.hpp"

#include <exception>
#include <gpioplus/chip.hpp>
#include <iostream>
#include <optional>
#include <phosphor-logging/elog-errors.hpp>

constexpr auto DBUS_OBJECT_NAME = "/xyz/openbmc_project/Chassis/Control/Power";
constexpr auto DBUS_INTF_NAME = "xyz.openbmc_project.Chassis.Control.Power";

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr,
                " [chip id] [pgood line offset] [pwrbtn line offset (active low)]\n");
        return 1;
    }

    try
    {
        phosphor::logging::log<phosphor::logging::level::INFO>(
            "Starting system power control service...");

        // Get command line arguments
        unsigned chip_id = std::stoi(argv[1]);
        uint32_t pgood_line_offset = std::stoi(argv[2]);
        uint32_t pwrbtn_line_offset = std::stoi(argv[3]);

        // Initialize GPIO
        gpioplus::Chip chip(chip_id);

        // Get PGood GPIO pin
        gpioplus::HandleFlags pgood_handle_flags(
            chip.getLineInfo(pgood_line_offset).flags);
        pgood_handle_flags.output = false;
        gpioplus::EventFlags pgood_event_flags;
        pgood_event_flags.rising_edge = true;
        pgood_event_flags.falling_edge = true;
        gpioplus::Event pgood_event(chip, pgood_line_offset, pgood_handle_flags,
                                    pgood_event_flags, "sys_power_control");

        // Get PwrBtn GPIO pin
        gpioplus::Handle::Line pwrbtn_line;
        pwrbtn_line.offset = pwrbtn_line_offset;
        pwrbtn_line.default_value = 0;
        gpioplus::HandleFlags pwrbtn_handle_flags(
            chip.getLineInfo(pwrbtn_line_offset).flags);
        pwrbtn_handle_flags.output = true;
        pwrbtn_handle_flags.active_low = true;
        gpioplus::Handle pwrbtn_handle(chip, {pwrbtn_line}, pwrbtn_handle_flags,
                                       "sys_power_control");

        // Initialize D-Bus
        sdbusplus::bus::bus bus = sdbusplus::bus::new_default();
        auto obj_path_inst = std::string{DBUS_OBJECT_NAME} + '0';
        sdbusplus::server::manager_t obj_manager(bus, obj_path_inst.c_str());
        sdeventplus::Event sd_event = sdeventplus::Event::get_default();

        // Initialize PowerControl component
        power_control::PowerControl power_control(
            bus, DBUS_INTF_NAME, obj_path_inst.c_str(), sd_event, pgood_event,
            pwrbtn_handle);

        // Start D-Bus processing
        bus.request_name(DBUS_INTF_NAME);
        bus.attach_event(sd_event.get(), SD_EVENT_PRIORITY_NORMAL);
        return sd_event.loop();
    }
    catch (std::exception& e)
    {
        phosphor::logging::log<phosphor::logging::level::ERR>(e.what());
        return -1;
    }
}
