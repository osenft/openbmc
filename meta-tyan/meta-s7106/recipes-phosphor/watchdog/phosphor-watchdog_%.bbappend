# Provide custom target and configuration
FILESEXTRAPATHS_prepend_s7106 := "${THISDIR}/${PN}:"

# Fix it so that it starts the watchdog on start.
SYSTEMD_ENVIRONMENT_FILE_${PN}_append_s7106 = " obmc/watchdog/host0"
OBMC_HOST_WATCHDOG_INSTANCES_A = "host0"
WATCHDOG_FMT_A = "../phosphor-watchdog@.service:multi-user.target.wants/phosphor-watchdog@{0}.service"
SYSTEMD_LINK_${PN}_append_s7106 := " ${@compose_list(d, 'WATCHDOG_FMT_A', 'OBMC_HOST_WATCHDOG_INSTANCES_A')}"
