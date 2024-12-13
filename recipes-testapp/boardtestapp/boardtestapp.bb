DESCRIPTION = "My custom application"
LICENSE = "CLOSED"
SRC_URI = "file://boardtestapp.pro \
           file://boardtestapp.service \
           file://sshd.service \
           file://sw_app_config.h \
           file://OpenCVWindow.h \
           file://opencvcamerathread.h \
           file://hotplugworkthread.h \
           file://mainwindow.h \
           file://osdeventwork.h \
           file://wirelessdeviceworkthread.h \
           file://main.cpp \           
           file://mainwindow.cpp \
           file://OpenCVWindow.cpp \
           file://opencvcamerathread.cpp \
           file://hotplugworkthread.cpp \
           file://osdeventwork.cpp \
           file://wirelessdeviceworkthread.cpp"

DEPENDS += "qtbase qtdeclarative qtmultimedia qtconnectivity opencv gstreamer1.0"

inherit qmake5
# systemd

S = "${WORKDIR}"

do_install() {
    install -d ${D}${bindir}
    # install -d ${D}${systemd_system_unitdir}
    install -m 0755 boardtestapp ${D}${bindir}
    # install -m 0755 ${WORKDIR}/boardtestapp.service ${D}${systemd_system_unitdir}
    # install -m 0755 ${WORKDIR}/sshd.service ${D}${systemd_unitdir}/system/
}

# SYSTEMD_SERVICE:${PN} = "boardtestapp.service"
# SYSTEMD_SERVICE = "sshd.service"
# SYSTEMD_AUTO_ENABLE = "enable"

# start sshd service
# pkg_postinst_ontarget:${PN} () {
#    if [ -x /bin/systemctl ]; then
#        /bin/systemctl enable sshd.service
#        /bin/systemctl start sshd.service
#    fi
# }