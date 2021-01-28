FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

RDEPENDS_${PN} += "bash"

do_install_append() {
  rm -f ${D}${sysconfdir}/${BPN}/server.ttyVUART0.conf
  install -d ${D}${sysconfdir}/${BPN}
  ln -sr ${D}${sysconfdir}/${BPN}.conf ${D}${sysconfdir}/${BPN}/server.${OBMC_CONSOLE_HOST_TTY}.conf
}

