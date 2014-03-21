#-------------------------------------------------
#
# Project created by QtCreator 2013-05-08T16:25:36
#
#-------------------------------------------------

QT       -= gui
QT       += network

TARGET = PhilipsRemote
TEMPLATE = lib

DEFINES += PHILIPSREMOTE_LIBRARY

SOURCES += philipsremote.cpp \
    ../AnotherRemoteServer/PluginBase.cpp

HEADERS += philipsremote.h\
        philipsremote_global.h \
    ../AnotherRemoteServer/PluginBase.hpp

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
