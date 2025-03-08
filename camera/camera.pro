QT       += core gui bluetooth multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /opt/st/stm32mp1/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi/usr/include

SOURCES += \
    ap_sensor.cpp \
    button.cpp \
    gps_read.cpp \
    hotplug.cpp \
    main.cpp \
    other.cpp \
    printer.cpp \
    showphoto.cpp \
    v4l2.cpp \
    widget.cpp

HEADERS += \
    ap_sensor.h \
    button.h \
    gps_read.h \
    hotplug.h \
    other.h \
    printer.h \
    showphoto.h \
    v4l2.h \
    widget.h

FORMS += \
    widget.ui

LIBS += -L -lavutil -lavformat -lavcodec -lswscale -lswresample
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
