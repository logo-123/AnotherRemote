#ifndef VIDEOLAN_H
#define VIDEOLAN_H

#include "VideoLAN_global.h"
#include "../AnotherRemoteServer/PluginBase.hpp"
#include <QString>
#if defined(Q_OS_LINUX)
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include "fixx11h.h"
#include <string.h>
#include <unistd.h>
#elif defined(Q_OS_WIN32)
#include <Windows.h>
#endif

class VIDEOLANSHARED_EXPORT VideoLAN : public PluginBase
{
public:
    VideoLAN();
    ~VideoLAN();

    void onPluginSelected();
    bool getPluginString(QString &buf);
    bool onAction(const int& num);
    bool onPlay();
    bool onStop();
    bool onNext();
    bool onPrev();
    bool onBack();
    bool onForw();

private:
#if defined(Q_OS_LINUX)
    Display* _display;
    Window _vlcWindow;

    void openDisplayWindow();
    void sendKeyStokes(unsigned long key, unsigned long modifier=0);
    void EnumerateWindows(Display *display, Window rootWindow);
#elif defined(Q_OS_WIN32)
    static HWND _vlcHandle;
    bool GetHandle();
    void sendKeyStokes(WORD modifier, WORD key);
    void sendKey(WORD key);
#endif
};

#endif // VIDEOLAN_H
