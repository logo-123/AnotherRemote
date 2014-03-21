#-------------------------------------------------
#
# Project created by QtCreator 2013-02-25T18:34:14
#
#-------------------------------------------------

QT       += core gui network dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AnotherRemoteServer
TEMPLATE = app

FORMS    += mainwindow.ui

RESOURCES += \
    ImageResource.qrc

unix {
   LIBS += -lasound -lXtst -lX11
   INCLUDEPATH += /usr/include/libappindicator-0.1 \
   /usr/include/gtk-2.0 \
   /usr/lib/gtk-2.0/include \
   /usr/include/glib-2.0 \
   /usr/lib/glib-2.0/include \
   /usr/include/cairo \
   /usr/include/atk-1.0 \
   /usr/include/pango-1.0
   LIBS += -L/usr/lib -lappindicator -lnotify
   CONFIG += link_pkgconfig
   PKGCONFIG = gtk+-2.0
}

win32 {
   LIBS += -lAdvapi32 -lPowrProf -lUser32 -lWinmm -lOle32
   RC_FILE = AnotherRemote.rc
}

SOURCES += main.cpp\
        mainwindow.cpp \
    settingsmanager.cpp \
    pluginmanager.cpp \
    PluginBase.cpp \
    powercontrol.cpp \
    Audio/SoundManager.cpp \
    Audio/Mixer.cpp \
    Audio/EndPointVolume.cpp \
    Audio/Alsa.cpp \
    httpserver/qhttpserver.cpp \
    httpserver/qhttpresponse.cpp \
    httpserver/qhttprequest.cpp \
    httpserver/qhttpconnection.cpp \
    httpserver/http_parser.c \
    InstrumentsCollection.cpp

HEADERS  += mainwindow.h \
    settingsmanager.h \
    pluginmanager.h \
    PluginBase.hpp \
    powercontrol.h \
    Audio/SoundManager.h \
    Audio/Mixer.h \
    Audio/EndPointVolume.h \
    Audio/AudioController.h \
    Audio/Alsa.h \
    httpserver/qhttpserver.h \
    httpserver/qhttpresponse.h \
    httpserver/qhttprequest.h \
    httpserver/qhttpconnection.h \
    httpserver/http_parser.h \
    fixx11h.h \
    InstrumentsCollectionInterface.h \
    InstrumentsCollection.h
