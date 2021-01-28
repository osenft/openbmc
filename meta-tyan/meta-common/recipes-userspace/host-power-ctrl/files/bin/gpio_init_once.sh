#!/bin/bash
#
# TYAN GPIO configuration script.
#
# Exports and sets host-related GPIOs in userspace, before host powers on.
# If a service depends on the GPIOs being exported here, it should have
# After=host-gpio.service.

source /usr/libexec/tyan-host-power-control-common.sh

main() {
  set_gpio_direction 'BMC_RSTBTN_OUT_N'       'high'
}

# Exit without running main() if sourced
return 0 2>/dev/null

main "$@"
