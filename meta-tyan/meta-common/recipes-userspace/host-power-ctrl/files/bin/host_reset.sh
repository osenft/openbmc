#!/bin/bash
#
# TYAN host reset script.
#
# We do not support host warm reset, so always do a cold reset.

source /usr/libexec/tyan-host-power-control-common.sh

main() {
  echo "Resetting the host!" >&2
  set_gpio_direction 'BMC_RSTBTN_OUT_N' low
  sleep 1
  set_gpio_direction 'BMC_RSTBTN_OUT_N' high

  return 0
}

# Exit without running main() if sourced
return 0 2>/dev/null

main "$@"
