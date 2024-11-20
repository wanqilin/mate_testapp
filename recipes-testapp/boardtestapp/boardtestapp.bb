DESCRIPTION = "My custom application"
LICENSE = "CLOSED"
SRC_URI = "file://main.cpp \
           file://mainwindow.h \
           file://mainwindow.cpp \
           file://boardtestapp.service \
           file://boardtestapp.pro"

DEPENDS += "qtbase qtdeclarative qtmultimedia"

inherit qmake5 systemd

S = "${WORKDIR}"

do_install() {
    install -d ${D}${bindir}
    install -d ${D}${systemd_system_unitdir}
    install -m 0755 boardtestapp ${D}${bindir}
    install -m 0755 ${WORKDIR}/boardtestapp.service ${D}${systemd_system_unitdir}
}

SYSTEMD_SERVICE:${PN} = "boardtestapp.service"
SYSTEMD_AUTO_ENABLE = "enable"