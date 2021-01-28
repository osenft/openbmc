inherit obmc-phosphor-utils

# Allow async code to clear failing sensors.
EXTRA_OECONF_remove_s7106 = " --enable-negative-errno-on-fail"
EXTRA_OECONF_append_s7106 = " --enable-update-functional-on-fail"

FILESEXTRAPATHS_prepend_s7106 := "${THISDIR}/${PN}:"

S7106_ITEMS = ""

# ADC sensors
S7106_ITEMS += "iio-hwmon.conf iio-hwmon-battery.conf"

# Fans
S7106_ITEMS += "ahb/apb/pwm-tacho-controller@1e786000.conf"

# Temperature sensors
S7106_ITEMS += "ahb/apb/bus@1e78a000/i2c-bus@40/nct7802@28.conf"

# Power supplies
S7106_ITEMS += "ahb/apb/bus@1e78a000/i2c-bus@100/power-supply@58.conf"
S7106_ITEMS += "ahb/apb/bus@1e78a000/i2c-bus@100/power-supply@59.conf"

S7106_ENVS = "obmc/hwmon/{0}"
SYSTEMD_ENVIRONMENT_FILE_${PN}_append_s7106 := " ${@compose_list(d, 'S7106_ENVS', 'S7106_ITEMS')}"
