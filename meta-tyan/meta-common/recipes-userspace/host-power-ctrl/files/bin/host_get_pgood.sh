#!/bin/bash
#
# TYAN host get "power good" script.

main() {
  busctl get-property \
    `mapper get-service /xyz/openbmc_project/Chassis/Control/Power0` \
    /xyz/openbmc_project/Chassis/Control/Power0 \
    xyz.openbmc_project.Chassis.Control.Power PGood
}

# Exit without running main() if sourced
return 0 2>/dev/null

main "$@"
