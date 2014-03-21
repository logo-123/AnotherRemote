#ifndef INSTRUMENTSCOLLECTION_H
#define INSTRUMENTSCOLLECTION_H

#include <QtGlobal>
#include <QString>

#include "InstrumentsCollectionInterface.h"

#if defined(Q_OS_LINUX)
#   include <X11/Xlib.h>
#   include <X11/extensions/XTest.h>
#   include "fixx11h.h"
#   include <string.h>
#   include <unistd.h>
#elif defined(Q_OS_WIN32)
#   include <windows.h>
#endif

class InstrumentsCollection
    : public InstrumentsCollectionInterface
{
public:
    InstrumentsCollection();
    ~InstrumentsCollection();

#if defined(Q_OS_LINUX)
    Status getErrorCode();
#elif defined(Q_OS_WIN32)
    DWORD getErrorCode();
#endif
    bool isErrored();

   bool sendKeyboardEvent(WindowHandle window, Keys key, Modifiers mod);
   bool mouseMoveRelative(int xDelta, int yDelta);
   bool mouseMoveStep(Mouse direction);
   bool mouseButtonEvent(Mouse button, bool down);
   bool mouseClick(Mouse button);
   WindowHandle getWindow(QString titleContaines, QString classIs);

private:
   typedef struct
   {
      QString windowName;
      QString className;
      WindowHandle handle;
   } callbackData;
#if defined(Q_OS_LINUX)
   WindowHandle searchWindow(const QString& nameContaines);
#elif defined(Q_OS_WIN32)
   static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
#endif
};

#endif // INSTRUMENTSCOLLECTION_H
