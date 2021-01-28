# We require the ASPEED PCI-to-AHB hardware implementation.
PACKAGECONFIG_append_s7106 = " aspeed-p2a"
IPMI_FLASH_BMC_ADDRESS_s7106 = "0x987F0000"
