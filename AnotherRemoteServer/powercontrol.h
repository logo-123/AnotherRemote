#ifndef POWERCONTROL_H
#define POWERCONTROL_H

#include <QtGlobal>

#if defined(Q_OS_LINUX)
#   include <QtDBus/QDBusConnection>
#elif defined(Q_OS_WIN32)
#   include <windows.h>
#else
#   error "Unsupported OS detected. Windows or Linux valid only"
#endif

class PowerControl
{
public:
    PowerControl();
    enum PowerStates{HIBERNATE = 0, POWEROFF, SLEEP};
    static void setPowerState(const PowerStates newState);

private:
};

#endif // POWERCONTROL_H
