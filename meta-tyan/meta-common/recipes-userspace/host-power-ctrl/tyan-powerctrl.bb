SUMMARY = "TYAN Power Control Services"
DESCRIPTION = "Systemd units and corresponding shell scripts to control host power."
LICENSE = "CLOSED"
PR = "r1"
PV = "0.1"

inherit systemd

S = "${WORKDIR}"
inherit meson pkgconfig
inherit obmc-phosphor-systemd
inherit pkgconfig
inherit systemd

SRC_URI = "file://bin/ \
           file://tyan-host-power-control-common.sh \
           file://meson.build \
           file://service-files \
           file://src \
          "

DEPENDS += "systemd"
DEPENDS += "autoconf-archive-native"
DEPENDS += "sdbusplus"
DEPENDS += "sdeventplus"
DEPENDS += "phosphor-dbus-interfaces"
DEPENDS += "phosphor-logging"
DEPENDS += "gpioplus"
RDEPENDS_${PN} = "bash"

FILES_${PN} += "${bindir}/*"
SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} += " tyan-init-gpios.service \
                           tyan-host-poweron.service \
                           tyan-host-poweroff.service \
                           tyan-host-powercycle.target \
                           tyan-host-ready.target \
                           tyan-host-reset.service \
			   sys_nreset_gpio_poller.service \
			   sys_power_control.service \
                         "

do_install_append() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/bin/* ${D}${bindir}/

    install -d ${D}${libexecdir}
    install -m 0755 ${S}/tyan-host-power-control-common.sh ${D}${libexecdir}/

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/service-files/*.service ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/service-files/*.target ${D}${systemd_system_unitdir}
}
