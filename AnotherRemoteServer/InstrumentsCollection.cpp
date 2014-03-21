#include <QDebug>
#include "InstrumentsCollection.h"

InstrumentsCollection::InstrumentsCollection()
    : InstrumentsCollectionInterface()
{
}

InstrumentsCollection::~InstrumentsCollection()
{
}

#if defined(Q_OS_LINUX)
Status InstrumentsCollection::getErrorCode()
{
    return _errCode;
}
#elif defined(Q_OS_WIN32)
DWORD InstrumentsCollection::getErrorCode()
{
    return _errCode;
}
#endif

bool InstrumentsCollection::isErrored()
{
    return (_errCode != 0);
}

bool InstrumentsCollection::sendKeyboardEvent(WindowHandle window, Keys key, Modifiers mod=ModNoKey)
{
    if (0 == window)
    {
        qDebug() << "Window not precified... Abort";
        return false;
    }
    if (key == KNoKey)
    {
        qDebug() << "Key not precified... Abort";
        return false;
    }
#if defined(Q_OS_LINUX)
    if (_display == 0)
    {
        qDebug() << "Display is null... Abort";
        return false;
    }
#endif

    bool ret = true;

    qDebug() << "Sending event Key " << key << " Modifier " << mod;

#if defined(Q_OS_LINUX)
    XEvent event;
    event.xkey.display     = _display;
    event.xkey.window      = window;
    event.xkey.time        = CurrentTime;
    event.xkey.x = event.xkey.y = event.xkey.x_root = event.xkey.y_root = 1;
    event.xkey.same_screen = False;
    event.xkey.keycode     = XKeysymToKeycode(_display, key);
    event.xkey.state       = mod;

    event.xkey.type = KeyPress;
    ret = ((_errCode = XSendEvent(_display, window, True, 0xfff, &event)) == 0);
    XFlush(_display);

    usleep(500);

    event.type = KeyRelease;
    ret = ret && ((_errCode = XSendEvent(_display, window, True, 0xfff, &event)) == 0);
    XFlush(_display);
#elif defined(Q_OS_WIN32)
    INPUT p;
    if (mod != ModNoKey)
    {
        memset(&p, 0, sizeof(INPUT));
        p.type = INPUT_KEYBOARD;
        p.ki.wVk = mod;
        ret = (SendInput(1, &p, sizeof(INPUT)) != 0);
        if (false == ret)
        {
            qDebug() << "Modifier down return: " << ret << " [" << GetLastError() << "]";
        }
    }
    if (ret)
    {
        qDebug() << "Sending key";
        SendMessage(window, WM_KEYDOWN, key, 0);
        ret = (GetLastError() == 0);
        if (false == ret)
        {
            qDebug() << "Key down return: " << ret << " [" << GetLastError() << "]";
        }
        SendMessage(window, WM_KEYUP, key, 0);
        ret = (GetLastError() == 0);
        if (false == ret)
        {
            qDebug() << "Key up return: " << ret << " [" << GetLastError() << "]";
        }
    }
    if (mod != ModNoKey)
    {
        p.ki.dwFlags = KEYEVENTF_KEYUP;
        ret = (SendInput(1, &p, sizeof(INPUT)) != 0);
        if (false == ret)
        {
            qDebug() << "Modifier up return: " << ret << " [" << GetLastError() << "]";
        }
    }
    _errCode = GetLastError();
#endif

    if (ret)
    {
        _errCode = 0;
    }

    return ret;
}

bool InstrumentsCollection::mouseMoveRelative(int xDelta, int yDelta)
{
    if ((xDelta == 0) && (yDelta == 0))
    {
        return true;
    }
#if defined(Q_OS_LINUX)
    if (_display == 0)
    {
        return false;
    }
#endif

    bool ret = true;

#if defined(Q_OS_LINUX)
    int ev, er, ma, mi;
    if(!XTestQueryExtension(_display, &ev, &er, &ma, &mi))
    {
        return false;
    }

    ret = ((_errCode = XTestFakeRelativeMotionEvent(_display, xDelta, yDelta, CurrentTime)) != 0);

    XFlush(_display);
#elif defined(Q_OS_WIN32)
    POINT p;

    GetCursorPos(&p);
    ret = (SetCursorPos(p.x + xDelta, p.y + yDelta) == 0);
    if (false == ret)
    {
        qDebug() << "Moving mouse to X " << p.x + xDelta << " Y " << p.y + yDelta
                 << " failed [" << GetLastError() << "]";
        _errCode = GetLastError();
    }
#endif

    if (ret)
    {
        _errCode = 0;
    }

    return ret;
}

bool InstrumentsCollection::mouseMoveStep(Mouse direction)
{
#if defined(Q_OS_LINUX)
    if (_display == 0)
    {
        return false;
    }
#endif

    bool ret = true;

#if defined(Q_OS_LINUX)
    int ev, er, ma, mi;
    if(!XTestQueryExtension(_display, &ev, &er, &ma, &mi))
    {
        return false;
    }

    switch (direction)
    {
    case MStepUp:
        ret = ((_errCode = XTestFakeRelativeMotionEvent(_display, 0, -_moveStep, CurrentTime)) == 0);
        break;
    case MStepDown:
        ret = ((_errCode = XTestFakeRelativeMotionEvent(_display, 0, _moveStep, CurrentTime)) == 0);
        break;
    case MStepLeft:
        ret = ((_errCode = XTestFakeRelativeMotionEvent(_display, -_moveStep, 0, CurrentTime)) == 0);
        break;
    case MStepRight:
        ret = ((_errCode = XTestFakeRelativeMotionEvent(_display, _moveStep, 0, CurrentTime)) == 0);
        break;
    default:
        break;
    }
    XFlush(_display);
#elif defined(Q_OS_WIN32)
    POINT p;
    GetCursorPos(&p);

    switch (direction)
    {
    case MStepUp:
        ret = SetCursorPos(p.x, p.y - _moveStep);
        break;
    case MStepDown:
        ret = SetCursorPos(p.x, p.y + _moveStep);
        break;
    case MStepLeft:
        ret = SetCursorPos(p.x - _moveStep, p.y);
        break;
    case MStepRight:
        ret = SetCursorPos(p.x + _moveStep, p.y);
        break;
    default:
        break;
    }
    _errCode = GetLastError();
#endif

    if (ret)
    {
        _errCode = 0;
    }

    return ret;
}

bool InstrumentsCollection::mouseButtonEvent(Mouse button, bool down)
{
#if defined(Q_OS_LINUX)
    if (_display == 0)
    {
        return false;
    }
#endif

    bool ret = true;
    _errCode = 0;

    if (down)
    {
#if defined(Q_OS_LINUX)
        XEvent event;
        memset(&event, 0x00, sizeof(event));

        qDebug() << "0";

        event.xbutton.button = static_cast<int> (button);
        event.xbutton.same_screen = True;
        if (true == down)
        {
            qDebug() << "1";
            event.type = ButtonPress;
        }
        else
        {
            qDebug() << "2";
            event.type = ButtonRelease;
            event.xbutton.state = 0x100;
        }

        qDebug() << "3";
        XQueryPointer(_display, RootWindow(_display, DefaultScreen(_display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
        event.xbutton.subwindow = event.xbutton.window;
        qDebug() << "4";
        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;
            XQueryPointer(_display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
            qDebug() << "5";
        }
        if(XSendEvent(_display, PointerWindow, True, 0xfff, &event) == 0)
        {
            qDebug() << "Error in XSendEvent";
        }
        XFlush(_display);
        qDebug() << "6";

        /*int ev, er, ma, mi;

        if(!XTestQueryExtension(_display, &ev, &er, &ma, &mi))
        {
            return false;
        }

        ret = (0 != (_errCode = XTestFakeButtonEvent(_display, static_cast<int> (button), True, CurrentTime)));
        XFlush(_display);*/
#elif defined(Q_OS_WIN32)
        DWORD type = static_cast<DWORD> (MNoKey);
        switch (button)
        {
        case MLeftButton:
            type = MOUSEEVENTF_LEFTDOWN;
            break;
        case MRightButton:
            type = MOUSEEVENTF_RIGHTDOWN;
            break;
        case MCentralButton:
            type = MOUSEEVENTF_MIDDLEDOWN;
            break;
        default:
            break;
        }

        if (type == MNoKey)
        {
            return false;
        }

        INPUT mouse = {0};
        mouse.type = INPUT_MOUSE;
        mouse.mi.dwFlags = type;
        ret = (SendInput(1, &mouse, sizeof(mouse)) != 0);
        if (false == ret)
        {
            qDebug() << "Mouse event failed [" << GetLastError() << "]";
        }
        _errCode = GetLastError();
#endif
    }
    else
    {
#if defined(Q_OS_LINUX)
        int ev, er, ma, mi;
        if(!XTestQueryExtension(_display, &ev, &er, &ma, &mi))
        {
            return false;
        }

        ret = (0 != (XTestFakeButtonEvent(_display, static_cast<int> (button), False, CurrentTime)));
        XFlush(_display);
#elif defined(Q_OS_WIN32)
        DWORD type = static_cast<DWORD> (MNoKey);
        switch (button)
        {
        case MLeftButton:
            type = MOUSEEVENTF_LEFTUP;
            break;
        case MRightButton:
            type = MOUSEEVENTF_RIGHTUP;
            break;
        case MCentralButton:
            type = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            break;
        }

        if (type == MNoKey)
        {
            return false;
        }

        INPUT mouse = {0};
        mouse.type = INPUT_MOUSE;
        mouse.mi.dwFlags = type;
        ret = (SendInput(1, &mouse, sizeof(mouse)) != 0);
        if (false == ret)
        {
            qDebug() << "Mouse event failed [" << GetLastError() << "]";
        }
        _errCode = GetLastError();
#endif
    }

    return ret;
}

bool InstrumentsCollection::mouseClick(Mouse button)
{
    bool ret = mouseButtonEvent(button, true);
    ret = ret && mouseButtonEvent(button, false);
    return ret;
}

// Class ignored in Linux
WindowHandle InstrumentsCollection::getWindow(QString titleContaines, QString classIs)
{
#if defined(Q_OS_LINUX)
   (void)(classIs);
   return searchWindow(titleContaines);
#elif defined(Q_OS_WIN32)
   InstrumentsCollection::callbackData cd;
   cd.windowName = titleContaines;
   cd.className = classIs;
   EnumWindows(InstrumentsCollection::EnumWindowsProc, reinterpret_cast<LPARAM> (&cd));
   return cd.handle;
#endif
}

#if defined(Q_OS_LINUX)
WindowHandle InstrumentsCollection::searchWindow(const QString& nameContaines)
{
   Atom a = XInternAtom(_display, "_NET_CLIENT_LIST", true);
	Atom actualType;
	int format;
	unsigned long numItems, bytesAfter;
	unsigned char *data =0;
	int status = XGetWindowProperty(_display, DefaultRootWindow(_display), a, 0L, (~0L),
      false, AnyPropertyType, &actualType, &format, &numItems, &bytesAfter, &data);

	if (status >= Success && numItems)
	{
		// success - we have data: Format should always be 32:
		if (format != 32)
      {
         qDebug() << "Invalid format";
         return 0;
      }
		// cast to proper format, and iterate through values:
		quint32 *array = (quint32*) data;
		for (quint32 k = 0; k < numItems; k++)
		{
			Window w = (Window)array[k];
         char* name = '\0';
         status = XFetchName(_display, w, &name);
         if (status >= Success)
         {
            qDebug() << "Found window [" << w << "]:" << name;

            if (0 != strstr(name, nameContaines.toLatin1().data()))
            {
               return w;
            }
         }
         XFree(name);
		}
		XFree(data);
	}
   
   return 0;
}
#elif defined(Q_OS_WIN32)
BOOL CALLBACK InstrumentsCollection::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    InstrumentsCollection::callbackData* data = reinterpret_cast
            <InstrumentsCollection::callbackData*> (lParam);

    if (((true == data->windowName.isEmpty()) && (true == data->className.isEmpty())) || (0 == data) ||
            (0 == hWnd))
    {
        return TRUE;
    }

    bool isWndNameValid = false;
    bool isClassNameValid = false;
    int requiredLevel = 0;

    if (false == data->windowName.isEmpty())
    {
        requiredLevel = 1;
        char* wndTitle = new char[1000] ();
        GetWindowTextA(hWnd, wndTitle, 1000);

        if (0 != wndTitle)
        {
            if (0 < strlen(wndTitle))
            {
                if (0 != strstr(wndTitle, data->windowName.toAscii().data()))
                {
                    isWndNameValid = true;
                }
            }
        }

        delete wndTitle;
        wndTitle = 0;
    }

    if (false == data->className.isEmpty())
    {
        requiredLevel += 2;
        char* wndClassName = new char[1000] ();
        GetClassNameA(hWnd, wndClassName, 1000);

        if (0 != wndClassName)
        {
            if (0 < strlen(wndClassName))
            {
                if (0 != strstr(wndClassName, data->className.toAscii().data()))
                {
                    if (true == data->className.contains(QString(wndClassName)))
                    {
                        isClassNameValid = true;
                    }
                }
            }
        }

        delete wndClassName;
        wndClassName = 0;
    }

    switch (requiredLevel)
    {
    case 1:
        if (true == isWndNameValid)
        {
            data->handle = hWnd;
            return FALSE;
        }
        break;
    case 2:
        if (true == isClassNameValid)
        {
            data->handle = hWnd;
            return FALSE;
        }
        break;
    case 3:
        if ((true == isWndNameValid) && (true == isClassNameValid))
        {
            data->handle = hWnd;
            return FALSE;
        }
    default:
        return TRUE;
        break;
    }

    return TRUE;
}
#endif
