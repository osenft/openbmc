LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

ALLOW_EMPTY_${PN} = "1"

PROVIDES += "virtual/obmc-settings-mgmt"
RPROVIDES_${PN} += "virtual-obmc-settings-mgmt"

PROVIDES += "virtual/obmc-user-mgmt"
RPROVIDES_${PN} += "virtual-obmc-user-mgmt"

PROVIDES += "virtual/obmc-fan-mgmt"
RPROVIDES_${PN} += "virtual-obmc-fan-mgmt"

