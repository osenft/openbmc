OBMC_MACHINE_FEATURES += "\
        obmc-phosphor-fan-mgmt \
        obmc-phosphor-flash-mgmt \
        obmc-host-ipmi \
        obmc-host-state-mgmt \
        obmc-chassis-state-mgmt \
        obmc-bmc-state-mgmt \
        "

PREFERRED_PROVIDER_virtual/obmc-host-ipmi-hw = "phosphor-ipmi-kcs"
PREFERRED_PROVIDER_virtual/phosphor-ipmi-fru-inventory = "tyan-ipmi-fru-inventory-native"
PREFERRED_PROVIDER_virtual/phosphor-ipmi-fru-properties = "tyan-ipmi-fru-properties-native"

