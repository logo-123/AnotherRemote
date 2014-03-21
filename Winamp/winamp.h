#ifndef WINAMP_H
#define WINAMP_H

#include "winamp_global.h"
#include "../AnotherRemoteServer/PluginBase.hpp"
#include <windows.h>

class WINAMPSHARED_EXPORT Winamp : public PluginBase
{
public:
    Winamp();
    ~Winamp();

    void onPluginSelected();
    bool getPluginString(QString& buf);
    bool onAction(const int& num);
    bool onPlay();
    bool onStop();
    bool onNext();
    bool onPrev();
    bool onBack();
    bool onForw();

private:
    HWND _winampHandle;
    void findWindow();
};

#endif // WINAMP_H
