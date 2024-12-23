QT += core gui multimedia multimediawidgets network bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    DEFINES += OS_WINDOWS
    INCLUDEPATH += D:/work/OpenCV/opencv-4.5.1/QT-opencv-4.5.1/build/install/include
    LIBS += -LD:/work/OpenCV/opencv-4.5.1/QT-opencv-4.5.1/build/install/x64/mingw/lib \
            -lopencv_core451 \
            -lopencv_imgproc451 \
            -lopencv_highgui451 \
            -lopencv_imgcodecs451 \
            -lopencv_videoio451 \
            -lopencv_objdetect451 \
            -lsetupapi \
            -liphlpapi \
            -lws2_32
} else:unix {
    DEFINES += OS_UNIX
    INCLUDEPATH += /usr/include/opencv4 \
                   /mnt/disk2/nxp/boundarydeviceEx/build/tmp/sysroots-components/cortexa53-mx8mp/gstreamer1.0/usr/include/gstreamer-1.0 \
                   /mnt/disk2/nxp/boundarydeviceEx/build/tmp/sysroots-components/cortexa53/glib-2.0/usr/include/glib-2.0 \
                   /mnt/disk2/nxp/boundarydeviceEx/build/tmp/sysroots-components/cortexa53/glib-2.0/usr/lib/glib-2.0/include

    LIBS += -L/usr/lib \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_imgcodecs \
            -lopencv_videoio \
            -lopencv_objdetect \
            -lgstreamer-1.0 \
            -lgobject-2.0 \
            -lglib-2.0
}

SOURCES += \
    # OpenCVWindow.cpp \
    hotplugworkthread.cpp \
    main.cpp \
    mainwindow.cpp \
    opencvcamerathread.cpp \
    osdeventwork.cpp \
    wirelessdeviceworkthread.cpp

HEADERS += \
    # OpenCVWindow.h \
    hotplugworkthread.h \
    mainwindow.h \
    opencvcamerathread.h \
    osdeventwork.h \
    sw_app_config.h \
    wirelessdeviceworkthread.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
