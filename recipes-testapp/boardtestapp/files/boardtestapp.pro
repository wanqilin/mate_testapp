QT += core gui multimedia multimediawidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
            -lsetupapi
} else:unix {
    DEFINES += OS_UNIX
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/lib \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_imgcodecs \
            -lopencv_videoio \
            -lopencv_objdetect
}

SOURCES += \
    # OpenCVWindow.cpp \
    main.cpp \
    mainwindow.cpp \
    opencvfacerecognition.cpp \
    osdupdatethread.cpp

HEADERS += \
    # OpenCVWindow.h \
    mainwindow.h \
    opencvfacerecognition.h \
    osdupdatethread.h \
    sw_app_config.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
