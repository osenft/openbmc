FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"
SRC_URI_append = " \
 file://aspeed-p2a-ctrl.h \
 "

do_install_aspeed_p2a_header () {
 cp ${WORKDIR}/aspeed-p2a-ctrl.h ${S}/include/uapi/linux/.
}

addtask install_aspeed_p2a_header after do_patch before do_configure
