# Enable Video output
OBMC_IMAGE_EXTRA_INSTALL_append = " fbterm"
OBMC_IMAGE_EXTRA_INSTALL_append = " liberation-fonts"
OBMC_IMAGE_EXTRA_INSTALL_append = " tyan-early-vga"
OBMC_IMAGE_EXTRA_INSTALL_append = " uart-render-controller"

# Power control
OBMC_IMAGE_EXTRA_INSTALL_append = " tyan-powerctrl"

# FRU tools
OBMC_IMAGE_EXTRA_INSTALL_append = " tyan-ipmi-fru"
