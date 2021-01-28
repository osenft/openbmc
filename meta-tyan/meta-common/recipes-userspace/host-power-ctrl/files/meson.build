project('tyan-powerctrl', 'cpp',
        version: '0.1', meson_version: '>=0.49.0',
	default_options: [
	  'warning_level=3',
	  'werror=true',
	  'cpp_std=c++17'
	])

gpioplus = dependency('gpioplus')
phosphor_dbus_interfaces = dependency('phosphor-dbus-interfaces')
phosphor_logging = dependency('phosphor-logging')
sdbusplus = dependency('sdbusplus')
sdeventplus = dependency('sdeventplus')
systemd = dependency('libsystemd', version : '>=221')

executable(
  'sys_power_control',
  'src/power_button.cpp',
  'src/power_control.cpp',
  'src/power_state.cpp',
  'src/sys_power_control_main.cpp',
  'src/systemd_helper.cpp',
  implicit_include_directories: false,
  dependencies: [
    gpioplus,
    phosphor_logging,
    phosphor_dbus_interfaces,
    sdbusplus,
    sdeventplus,
    systemd,
  ],
  install: true,
  install_dir: get_option('bindir')
)

executable(
  'sys_nreset_gpio_poller',
  'src/sys_nreset_gpio_poller.cpp',
  implicit_include_directories: false,
  dependencies: [
    gpioplus,
    phosphor_logging,
    phosphor_dbus_interfaces,
    sdbusplus,
    sdeventplus,
    systemd,
  ],
  install: true,
  install_dir: get_option('bindir')
)
