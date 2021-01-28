SUMMARY = "TYAN IPMI FRU Services"
DESCRIPTION = "Systemd units and corresponding shell scripts to load TYAN FRU."
LICENSE = "CLOSED"
PR = "r1"
PV = "0.1"

inherit systemd
inherit obmc-phosphor-systemd

S = "${WORKDIR}"

SRC_URI = "file://tyan-read-eeprom.sh \
           file://system-blank.bin \
          "

DEPENDS = "systemd"
RDEPENDS_${PN} = "bash \
                  phosphor-ipmi-fru \
                 "

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE_${PN} += " tyan-read-eeprom@.service"
SYSTEMD_ENVIRONMENT_FILE_${PN}_append := " \
  obmc/eeproms/bmc \
  obmc/eeproms/system \
"

SYSTEMD_LINK_${PN}_append := " \
  ../tyan-read-eeprom@.service:tyan-host-ready.target.wants/tyan-read-eeprom@bmc.service \
  ../tyan-read-eeprom@.service:tyan-host-ready.target.wants/tyan-read-eeprom@system.service \
"

FILES_${PN}_append = " \
  ${datadir}/ipmi-fru/ \
"

do_install_append() {
    install -d ${D}${datadir}/ipmi-fru
    install -m 0555 ${S}/system-blank.bin ${D}${datadir}/ipmi-fru/
    install -d ${D}${bindir}
    install -m 0755 ${S}/tyan-read-eeprom.sh ${D}${bindir}/
}
