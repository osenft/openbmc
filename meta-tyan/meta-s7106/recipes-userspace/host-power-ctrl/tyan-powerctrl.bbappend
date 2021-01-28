FILESEXTRAPATHS_prepend_s7106 := "${THISDIR}/files:"
SYSTEMD_ENVIRONMENT_FILE_${PN}_s7106 += "obmc/sys_nreset_gpio_poller"
SYSTEMD_ENVIRONMENT_FILE_${PN}_s7106 += "obmc/sys_power_control"
