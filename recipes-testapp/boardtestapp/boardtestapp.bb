DESCRIPTION = "My custom application"
LICENSE = "CLOSED"
SRC_URI = "file://boardtestapp.pro \
           file://boardtestapp.service \
           file://sw_app_config.h \
           file://OpenCVWindow.h \
           file://opencvfacerecognition.h \
           file://osdupdatethread.h \
           file://mainwindow.h \
           file://main.cpp \           
           file://mainwindow.cpp \
           file://OpenCVWindow.cpp \
           file://opencvfacerecognition.cpp \
           file://osdupdatethread.cpp "

DEPENDS += "qtbase qtdeclarative qtmultimedia opencv"

inherit qmake5
# systemd

S = "${WORKDIR}"


do_install() {
    install -d ${D}${bindir}
    # install -d ${D}${systemd_system_unitdir}
    install -m 0755 boardtestapp ${D}${bindir}
    # install -m 0755 ${WORKDIR}/boardtestapp.service ${D}${systemd_system_unitdir}
}

# SYSTEMD_SERVICE:${PN} = "boardtestapp.service"
# SYSTEMD_AUTO_ENABLE = "enable"