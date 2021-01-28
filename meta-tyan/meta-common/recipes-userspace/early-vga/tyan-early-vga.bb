SUMMARY = "Early VGA Service"
DESCRIPTION = "Shell scripts to enable early VGA."
LICENSE = "CLOSED"
PR = "r1"
PV = "0.1"

S = "${WORKDIR}"
SRC_URI = "file://host-booting-screen.sh"
RDEPENDS_${PN} = "bash phosphor-host-postd"

FILES_${PN} += "${bindir}/*"

do_install_append() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/host-booting-screen.sh ${D}${bindir}/
}
