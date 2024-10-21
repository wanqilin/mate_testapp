DESCRIPTION = "My custom application"
LICENSE = "CLOSED"
SRC_URI = "file://main.cpp \
           file://mainwindow.h \
           file://mainwindow.cpp \
           file://boardtestapp.pro"

DEPENDS += "qtbase qtdeclarative qtmultimedia"

inherit qmake5

S = "${WORKDIR}"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 boardtestapp ${D}${bindir}
}

