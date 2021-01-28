/*
 * Polls a GPIO (nRESET) and updates
 * xyz.openbmc_project.State.Host.CurrentHostState
 * - to Running when GPIO has a rising edge and the state was Quiesced before
 * - to Quiesced when the GPIO has a falling edge and the state was Running
 *   before
 */

#include <exception>
#include <gpioplus/chip.hpp>
#include <gpioplus/event.hpp>
#include <gpioplus/handle.hpp>
#include <iostream>
#include <sdbusplus/bus.hpp>
#include <string>

constexpr auto HOST_BUSNAME = "xyz.openbmc_project.State.Host";
constexpr auto HOST_PATH = "/xyz/openbmc_project/state/host0";
constexpr auto HOST_OBJNAME = "xyz.openbmc_project.State.Host";
constexpr auto HOST_STATE_PROPNAME = "CurrentHostState";
constexpr auto HOST_STATE_VALUE_QUIESCED =
    "xyz.openbmc_project.State.Host.HostState.Quiesced";
constexpr auto HOST_STATE_VALUE_RUNNING =
    "xyz.openbmc_project.State.Host.HostState.Running";

constexpr auto PROP_INTERFACE = "org.freedesktop.DBus.Properties";
constexpr auto PROP_METHOD_GET = "Get";
constexpr auto PROP_METHOD_SET = "Set";

static std::string GetProperty(sdbusplus::bus::bus& bus, const char* busName,
                               const char* pathName, const char* objName,
                               const char* propName)
{
    auto method =
        bus.new_method_call(busName, pathName, PROP_INTERFACE, PROP_METHOD_GET);

    method.append(objName, propName);

    std::variant<std::string> value;
    auto result = bus.call(method);
    result.read(value);

    return std::get<std::string>(value);
}

static void SetProperty(sdbusplus::bus::bus& bus, const char* busName,
                        const char* pathName, const char* objName,
                        const char* propName, const char* value)
{
    auto method =
        bus.new_method_call(busName, pathName, PROP_INTERFACE, PROP_METHOD_SET);

    method.append(objName, propName, std::variant<std::string>(value));
    bus.call(method);
}

static std::string GetHostState(sdbusplus::bus::bus& bus)
{
    return GetProperty(bus, HOST_BUSNAME, HOST_PATH, HOST_OBJNAME,
                       HOST_STATE_PROPNAME);
}

static void SetHostState(sdbusplus::bus::bus& bus, const char* value)
{
    SetProperty(bus, HOST_BUSNAME, HOST_PATH, HOST_OBJNAME, HOST_STATE_PROPNAME,
                value);
}

void GpioValueHandler(sdbusplus::bus::bus& bus, int value)
{
    std::string hostState = GetHostState(bus);

    std::cout << "GpioValueHandler(value=" << value
              << ", hostState=" << hostState << ")" << std::endl;
    if (value == 0)
    {
        if (hostState == HOST_STATE_VALUE_QUIESCED)
        {
            SetHostState(bus, HOST_STATE_VALUE_RUNNING);
        }
    }
    else
    {
        if (hostState == HOST_STATE_VALUE_RUNNING)
        {
            SetHostState(bus, HOST_STATE_VALUE_QUIESCED);
        }
    }
}

int PollGpio(gpioplus::Event& event)
{
    auto bus = sdbusplus::bus::new_default();

    // Ignore initial value, we only care about changes.

    while (1)
    {
        auto data = event.read();
        if (!data)
            continue;

        std::cout << "Event " << data->id << std::endl;
        GpioValueHandler(bus, data->id == GPIOEVENT_EVENT_RISING_EDGE);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, " [chip id] [line offset (active low)]\n");
        return 1;
    }

    try
    {
        unsigned chip_id = std::stoi(argv[1]);
        uint32_t line_offset = std::stoi(argv[2]);

        gpioplus::Chip chip(chip_id);
        gpioplus::HandleFlags handle_flags(chip.getLineInfo(line_offset).flags);
        handle_flags.output = false;
        handle_flags.active_low = true;

        gpioplus::EventFlags event_flags;
        event_flags.rising_edge = true;
        event_flags.falling_edge = true;

        gpioplus::Event event(chip, line_offset, handle_flags, event_flags,
                              "sys_nreset_gpio_poller");
        return PollGpio(event);
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
    }
    return 1;
}
