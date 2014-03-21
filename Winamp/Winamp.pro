#-------------------------------------------------
#
# Project created by QtCreator 2013-05-07T12:53:45
#
#-------------------------------------------------

QT       -= gui

TARGET = Winamp
TEMPLATE = lib

DEFINES += WINAMP_LIBRARY

LIBS += -luser32

SOURCES += winamp.cpp \
    ../AnotherRemoteServer/PluginBase.cpp

HEADERS += winamp.h\
        winamp_global.h \
    ../AnotherRemoteServer/PluginBase.hpp \
    winamp_api.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
