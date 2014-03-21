#ifndef INSTRUMENTCLUSTERINTERFACE_H
#define INSTRUMENTCLUSTERINTERFACE_H

#include <QtGlobal>
#include <QString>

#if defined(Q_OS_LINUX)
#   include <X11/Xlib.h>
#   include <X11/extensions/XTest.h>
#   include "fixx11h.h"
#   include <string.h>
#   include <unistd.h>
#   include <X11/keysym.h>
#elif defined(Q_OS_WIN32)
#   include <windows.h>
#endif

#if defined(Q_OS_LINUX)
typedef Window WindowHandle;
#elif defined(Q_OS_WIN32)
typedef HWND WindowHandle;
#endif

class InstrumentsCollectionInterface
{
public:
    enum Mouse {MNoKey=0x00, MLeftButton=1, MRightButton=2, MCentralButton=3, MStepUp=4, MStepDown=5,
               MStepRight=6, MStepLeft=7};
#if defined(Q_OS_LINUX)
    enum Modifiers {ModNoKey=0x00, ModAlt=Mod1Mask, ModCtrl=ControlMask, ModShift=ShiftMask};

    enum Keys {KNoKey=0x00, KBackspace=XK_BackSpace, KTab=XK_Tab, KClear=XK_Clear, KReturn=XK_Return,
               KPause=XK_Pause, KEscape=XK_Escape, KSpacebar=XK_KP_Space, KPageUp=XK_Page_Up,
               KPageDown=XK_Page_Down, KEnd=XK_End, KHome=XK_Home, KLeft=XK_Left, KUp=XK_Up,
               KRight=XK_Right, KDown=XK_Down, KSelect=XK_Select, KPrint=XK_Print, KExecute=XK_Execute,
               KInsert=XK_Insert, KDelete=XK_Delete, K_0=XK_0,
               K_1=XK_1, K_2=XK_2, K_3=XK_3, K4=XK_4, K_5=XK_5, K_6=XK_6, K_7=XK_7, K_8=XK_8, K_9=XK_9,
               K_A=XK_A, K_B=XK_B, K_C=XK_C, K_D=XK_D, K_E=XK_E, K_F=XK_F, K_G=XK_G, K_H=XK_H, K_I=XK_I,
               K_J=XK_J, K_K=XK_K, K_L=XK_L, K_M=XK_M, K_N=XK_N, K_O=XK_O, K_P=XK_P, K_Q=XK_Q, K_R=XK_R,
               K_S=XK_S, K_T=XK_T, K_U=XK_U, K_V=XK_V, K_W=XK_W, K_X=XK_X, K_Y=XK_Y, K_Z=XK_Z,
               KNum_0=XK_KP_0, KNum_1=XK_KP_1, KNum_2=XK_KP_2, KNum_3=XK_KP_3, KNum_4=XK_KP_4,
               KNum_5=XK_KP_5, KNum_6=XK_KP_6, KNum_7=XK_KP_7, KNum_8=XK_KP_8, KNum_9=XK_KP_9,
               KMultiply=XK_KP_Multiply, KAdd=XK_KP_Add, KSeparator=XK_KP_Separator, KSubtract=XK_KP_Subtract,
               KDecimal=XK_KP_Decimal, KDivide=XK_KP_Divide, KF1=XK_F1, KF2=XK_F2, KF3=3, KF4=XK_F4, KF5=XK_F5,
               KF6=XK_F6, KF7=XK_F7, KF8=XK_F8, KF9=XK_F9, KF10=XK_F10, KF11=XK_F11, KF12=XK_F12};
#elif defined(Q_OS_WIN32)
    enum Modifiers {ModNoKey=0x00, ModAlt=VK_MENU, ModCtrl=VK_CONTROL, ModShift=VK_SHIFT};

    enum Keys {KNoKey=0x00, KBackspace=VK_BACK, KTab=VK_TAB, KClear=VK_CLEAR, KReturn=VK_RETURN,
               KPause=VK_PAUSE, KEscape=VK_ESCAPE, KSpacebar=VK_SPACE, KPageUp=VK_PRIOR, KPageDown=VK_NEXT,
               KEnd=VK_END, KHome=VK_HOME, KLeft=VK_LEFT, KUp=VK_UP, KRight=VK_RIGHT, KDown=VK_DOWN,
               KSelect=VK_SELECT, KPrint=VK_PRINT, KExecute=VK_EXECUTE, KPrintScreen=VK_SNAPSHOT,
               KInsert=VK_INSERT, KDelete=VK_DELETE, K_0=0x30, K_1=0x31, K_2=0x32, K_3=0x33, K4=0x34,
               K_5=0x35, K_6=0x36, K_7=0x37, K_8=0x38, K_9=0x39, K_A='A', K_B='B', K_C='C',
               K_D='D', K_E='E', K_F='F', K_G='G', K_H='H', K_I='I', K_J='J', K_K='K',
               K_L='L', K_M='M', K_N='N', K_O='O', K_P='P', K_Q='Q', K_R='R', K_S='S',
               K_T='T', K_U='U', K_V='V', K_W='W', K_X='X', K_Y='Y', K_Z='Z', KNum_0=VK_NUMPAD0,
               KNum_1=VK_NUMPAD1, KNum_2=VK_NUMPAD2, KNum_3=VK_NUMPAD3, KNum_4=VK_NUMPAD4, KNum_5=VK_NUMPAD5,
               KNum_6=VK_NUMPAD6, KNum_7=VK_NUMPAD7, KNum_8=VK_NUMPAD8, KNum_9=VK_NUMPAD9, KMultiply=VK_MULTIPLY,
               KAdd=VK_ADD, KSeparator=VK_SEPARATOR, KSubtract=VK_SUBTRACT, KDecimal=VK_DECIMAL, KDivide=VK_DIVIDE,
               KF1=VK_F1, KF2=VK_F2, KF3=VK_F3, KF4=VK_F4, KF5=VK_F5, KF6=VK_F6, KF7=VK_F7, KF8=VK_F8,
               KF9=VK_F9, KF10=VK_F10, KF11=VK_F11, KF12=VK_F12};
#endif

    InstrumentsCollectionInterface()
    #if defined(Q_OS_LINUX)
        : _display(0)
    #endif
    {
#if defined(Q_OS_LINUX)
        _display = XOpenDisplay(NULL);
#endif
    }

    virtual ~InstrumentsCollectionInterface()
    {
#if defined(Q_OS_LINUX)
        if (_display)
        {
            XCloseDisplay(_display);
        }
#endif
    }
#if defined(Q_OS_LINUX)
    virtual Status getErrorCode() = 0;
#elif defined(Q_OS_WIN32)
    virtual DWORD getErrorCode() = 0;
#endif
    virtual bool isErrored() = 0;

    virtual bool sendKeyboardEvent(WindowHandle window, Keys key, Modifiers mod) = 0;
    virtual bool mouseMoveRelative(int xDelta, int yDelta) = 0;
    virtual bool mouseMoveStep(Mouse direction) = 0;
    virtual bool mouseButtonEvent(Mouse button, bool down) = 0;
    virtual bool mouseClick(Mouse button) = 0;
    virtual WindowHandle getWindow(QString titleContaines, QString classIs) = 0;

    void setMouseStep(unsigned int step) {_moveStep = step;}

protected:
    int _moveStep;
#if defined(Q_OS_LINUX)
    Display *_display;
    Status _errCode;
#elif defined(Q_OS_WIN32)
    DWORD _errCode;
#endif
};

#endif // INSTRUMENTCLUSTERINTERFACE_H
