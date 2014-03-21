#include <QtGlobal>

#if defined(Q_OS_LINUX)
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#elif defined(Q_OS_WIN32)
#include <PowrProf.h>
#endif

#include "powercontrol.h"

PowerControl::PowerControl()
{
}

void PowerControl::setPowerState(const PowerStates newState)
{
#if defined(Q_OS_LINUX)
    QDBusInterface hint("org.freedesktop.UPower", "/org/freedesktop/UPower",
                       "org.freedesktop.UPower", QDBusConnection::systemBus());

    QDBusInterface pHint("org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Manager",
                         "org.freedesktop.ConsoleKit.Manager", QDBusConnection::systemBus());
#elif defined(Q_OS_WIN32)
    TOKEN_PRIVILEGES tkp;
    HANDLE hToken;
#endif

    switch(newState)
    {
    case PowerControl::HIBERNATE:
#if defined(Q_OS_LINUX)
        hint.call("Hibernate");
#elif defined(Q_OS_WIN32)
        SetSuspendState(true, false, false);
#endif
        break;
    case PowerControl::SLEEP:
#if defined(Q_OS_LINUX)
        hint.call("Suspend");
#elif defined(Q_OS_WIN32)
        SetSuspendState(false, false, false);
#endif
        break;
    case PowerControl::POWEROFF:
#if defined(Q_OS_LINUX)
        pHint.call("Stop");
#elif defined(Q_OS_WIN32)
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE, SHTDN_REASON_FLAG_PLANNED);
#endif
        break;
    default:
        break;
    }
}
