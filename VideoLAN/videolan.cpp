#include "videolan.h"
#include <QtGlobal>
#include <QDebug>

const char* const pName = "VideoLAN";
const char* const aName[8] = {"Volume +", "Volume -", "-1 min", "+1 min",
                               "Fullscreen", "Audio track", "Subtitle track", "Time code"};
#if defined(Q_OS_WIN32)
HWND VideoLAN::_vlcHandle = NULL;
#endif

VideoLAN::VideoLAN()
: PluginBase()
#if defined(Q_OS_LINUX)
, _display(0)
, _vlcWindow(0)
#endif
{
    _myData.setName(QString(pName));

    for (unsigned int i = 0; i < 8; ++i)
    {
        _myData.setActionName(i, QString(aName[i]));
    }
}

VideoLAN::~VideoLAN()
{
#if defined(Q_OS_LINUX)
    if (_display)
    {
        XCloseDisplay(_display);
    }
#endif
}

void VideoLAN::onPluginSelected()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
#elif defined(Q_OS_WIN32)
    GetHandle();
#endif
}

bool VideoLAN::getPluginString(QString& buf)
{
    buf.clear();
    return true;
}

bool VideoLAN::onAction(const int& num)
{
    qDebug() << "Get action " << num;

#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }

    switch (num)
    {
    case 1:
        sendKeyStokes(XK_Up, XK_Control_L);
        break;
    case 2:
        sendKeyStokes(XK_Down, XK_Control_L);
        break;
    case 3:
        sendKeyStokes(XK_Left, XK_Control_L);
        break;
    case 4:
        sendKeyStokes(XK_Right, XK_Control_L);
        break;
    case 5:
        sendKeyStokes(XK_F);
        break;
    case 6:
        sendKeyStokes(XK_B);
        break;
    case 7:
        sendKeyStokes(XK_V);
        break;
    case 8:
        sendKeyStokes(XK_T);
        break;
    default:
        break;
    }

    return true;
#elif defined(Q_OS_WIN32)

    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }

    switch (num)
    {
    case 1:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::KUp, InstrumentsCollectionInterface::ModCtrl);
        }
        break;
    case 2:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::KDown, InstrumentsCollectionInterface::ModCtrl);
        }
        break;
    case 3:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::KLeft, InstrumentsCollectionInterface::ModCtrl);
        }
        break;
    case 4:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::KRight, InstrumentsCollectionInterface::ModCtrl);
        }
        break;
    case 5:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::K_F, InstrumentsCollectionInterface::ModNoKey);
        }
        break;
    case 6:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::K_B, InstrumentsCollectionInterface::ModNoKey);
        }
        break;
    case 7:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::K_V, InstrumentsCollectionInterface::ModNoKey);
        }
        break;
    case 8:
        if (0 != _instrumentsCollection)
        {
            _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
                InstrumentsCollectionInterface::K_T, InstrumentsCollectionInterface::ModNoKey);
        }
        break;
    default:
        break;
    }

    return true;
#endif
}

bool VideoLAN::onPlay()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }
    sendKeyStokes(XK_space);
#elif defined(Q_OS_WIN32)
    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }
    if (0 != _instrumentsCollection)
    {
        _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
            InstrumentsCollectionInterface::KSpacebar, InstrumentsCollectionInterface::ModNoKey);
    }
#endif
    return true;
}

bool VideoLAN::onStop()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }
    sendKeyStokes(XK_S);
#elif defined(Q_OS_WIN32)
    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }
    if (0 != _instrumentsCollection)
    {
        _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
            InstrumentsCollectionInterface::K_S, InstrumentsCollectionInterface::ModNoKey);
    }
#endif
    return true;
}

bool VideoLAN::onNext()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }
    sendKeyStokes(XK_N);
#elif defined(Q_OS_WIN32)
    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }
    if (0 != _instrumentsCollection)
    {
        _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
            InstrumentsCollectionInterface::K_N, InstrumentsCollectionInterface::ModNoKey);
    }
#endif
    return true;
}

bool VideoLAN::onPrev()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }
    sendKeyStokes(XK_P);
#elif defined(Q_OS_WIN32)
    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }
    if (0 != _instrumentsCollection)
    {
        _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
            InstrumentsCollectionInterface::K_P, InstrumentsCollectionInterface::ModNoKey);
    }
#endif
    return true;
}

bool VideoLAN::onBack()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }
    sendKeyStokes(XK_Left, XK_Alt_L);
#elif defined(Q_OS_WIN32)
    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }
    if (0 != _instrumentsCollection)
    {
        _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
            InstrumentsCollectionInterface::KLeft, InstrumentsCollectionInterface::ModAlt);
    }
#endif
    return true;
}

bool VideoLAN::onForw()
{
#if defined(Q_OS_LINUX)
    openDisplayWindow();
    if (!_vlcWindow)
    {
        return false;
    }
    sendKeyStokes(XK_Right, XK_Alt_L);
#elif defined(Q_OS_WIN32)
    GetHandle();
    if (0 == _vlcHandle)
    {
        return false;
    }
    if (0 != _instrumentsCollection)
    {
        _instrumentsCollection->sendKeyboardEvent(_vlcHandle,
            InstrumentsCollectionInterface::KRight, InstrumentsCollectionInterface::ModAlt);
    }
#endif
    return true;
}

#if defined(Q_OS_LINUX)
void VideoLAN::EnumerateWindows(Display *display, Window rootWindow)
{
   Window parent;
   Window *children;
   unsigned int noOfChildren;
   int status;
   int i;
   XTextProperty wmName;
   char **list;

   status = XGetWMName(display, rootWindow, &wmName);
   if ((status) && (wmName.value) && (wmName.nitems))
   {
      status = XmbTextPropertyToTextList(display, &wmName, &list, &i);
      if ((status >= Success) && (i) && (*list))
      {
         if (QString(strdup(*list)).contains("VLC"))
         {
             _vlcWindow = rootWindow;
         }
      }
   }

   status = XQueryTree(display, rootWindow, &rootWindow, &parent, &children, &noOfChildren);

   if (status == 0)
   {
      return;
   }

   if (noOfChildren == 0)
   {
      return;
   }

   for (i = 0; i < static_cast<int> (noOfChildren); ++i)
   {
      EnumerateWindows(display, children[i]);
   }

   XFree(reinterpret_cast<char *> (children));
}

void VideoLAN::openDisplayWindow()
{
    if (!_display)
    {
        _display = XOpenDisplay(NULL);
    }
    if (!_vlcWindow)
    {
        EnumerateWindows(_display, RootWindow(_display, DefaultScreen(_display)));
    }
    if (_display)
    {
        if (!_vlcWindow)
        {
            qDebug() << "VideoLAN::openDisplayWindow VLC window not found";
        }
    }
    else
    {
        qDebug() << "VideoLAN::openDisplayWindow display return NULL";
    }
}

void VideoLAN::sendKeyStokes(unsigned long key, unsigned long modifier)
{
    XEvent event;

    memset(&event, 0, sizeof(event));

    unsigned int mod = 0;

    switch (modifier)
    {
    case XK_Control_L:
    case XK_Control_R:
        mod = ControlMask;
        break;
    case XK_Shift_L:
    case XK_Shift_R:
        mod = ShiftMask;
        break;
    case XK_Alt_L:
    case XK_Alt_R:
        mod = Mod1Mask;
        break;
    default:
        break;
    }

    event.xkey.display     = _display;
    event.xkey.window      = _vlcWindow;
    event.xkey.time        = CurrentTime;
    event.xkey.x = event.xkey.y = event.xkey.x_root = event.xkey.y_root = 1;
    event.xkey.same_screen = False;
    event.xkey.keycode     = XKeysymToKeycode(_display, key);
    event.xkey.state       = mod;

    event.xkey.type = KeyPress;
    XSendEvent(_display, _vlcWindow, True, 0xfff, &event);
    XFlush(_display);

    usleep(500);

    event.type = KeyRelease;
    XSendEvent(_display, _vlcWindow, True, 0xfff, &event);
    XFlush(_display);
}
#elif defined(Q_OS_WIN32)
void VideoLAN::sendKey(WORD key)
{
    SendMessage(_vlcHandle, WM_KEYDOWN, key, 0);
    SendMessage(_vlcHandle, WM_KEYUP, key, 0);
}

void VideoLAN::sendKeyStokes(WORD modifier, WORD key)
{
    INPUT p;
    memset(&p, 0, sizeof(INPUT));
    p.type = INPUT_KEYBOARD;
    p.ki.wVk = modifier;

    SendInput(1, &p, sizeof(INPUT));
    SendMessage(_vlcHandle, WM_KEYDOWN, key, 0);
    SendMessage(_vlcHandle, WM_KEYUP, key, 0);
    p.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &p, sizeof(INPUT));
}

bool VideoLAN::GetHandle()
{
    _vlcHandle = NULL;

    if (_instrumentsCollection != 0)
    {
        _vlcHandle = _instrumentsCollection->getWindow(QString("VLC"), QString("QWidget"));
    }

    return (_vlcHandle != NULL);
}
#endif

#if defined(Q_OS_LINUX)
extern "C" PluginBase *getInstance()
#elif defined(Q_OS_WIN32)
extern "C" __declspec(dllexport) PluginBase *getInstance()
#endif
{
    static PluginBase *inst = new VideoLAN();

    return inst;
}
