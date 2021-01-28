FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"
SRC_URI_append = " file://0001-Always-use-D-Bus-bus-.SMS.patch"
KCS_DEVICE = "ipmi-kcs1"
