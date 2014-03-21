#-------------------------------------------------
#
# Project created by QtCreator 2013-03-05T08:12:44
#
#-------------------------------------------------

QT       -= gui

TARGET = VideoLAN
TEMPLATE = lib

DEFINES += VIDEOLAN_LIBRARY

win32 {
    HEADERS +=
    LIBS += -lUser32
}

unix {
    LIBS += -lXtst
}

SOURCES += videolan.cpp \
    ../AnotherRemoteServer/PluginBase.cpp

HEADERS += videolan.h\
        VideoLAN_global.h \
    ../AnotherRemoteServer/PluginBase.hpp \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
