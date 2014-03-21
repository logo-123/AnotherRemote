#include "winamp.h"
#include "winamp_api.h"
#include <QString.h>

const char * const pName = "Winamp";
const char * const aName[8] = {"Volume +", "Volume -", "Start play", "+5 seconds",
                               "-5 seconds", "n/a", "n/a", "n/a"};

Winamp::Winamp()
    : PluginBase()
    , _winampHandle(0)
{
    _myData.setName(QString(pName));

    for (unsigned int i = 0; i < 8; ++i)
    {
        _myData.setActionName(i, QString(aName[i]));
    }
}

Winamp::~Winamp()
{
}

void Winamp::onPluginSelected()
{
    findWindow();
}

bool Winamp::getPluginString(QString &buf)
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    buf = "<font size=\"5\">";

    if(SendMessage(_winampHandle, WM_WA_IPC, 0, IPC_ISPLAYING) == 1)
    {
        char *cName = new char[260] ();
        GetWindowTextA(_winampHandle, cName, 259);
        QString fileName(cName);
        buf.append("Now playing: ");
        buf.append(fileName.mid(0, fileName.length()-9));
        delete [] cName;


        // Get track lenght
        int seconds = SendMessage(_winampHandle, WM_WA_IPC, 1, IPC_GETOUTPUTTIME);
        int minutes = 0;
        while(seconds >= 60)
        {
            minutes++;
            seconds = seconds - 60;
        }
        buf += "<br/>Play time: " + QString::number(minutes)+":"+QString::number(seconds);
        int sampRate = SendMessage(_winampHandle, WM_WA_IPC, 0, IPC_GETINFO);
        buf += "<br/>Sample rate: "+QString::number(sampRate)+" kHz";
        int bitRate = SendMessage(_winampHandle, WM_WA_IPC, 1, IPC_GETINFO);
        buf += "<br/>Bit rate: "+QString::number(bitRate)+" kbps";
        int playListLength = SendMessage(_winampHandle, WM_WA_IPC, 0, IPC_GETLISTLENGTH);
        buf += "<br/>Play list: "+QString::number(playListLength)+" positions";
        int playListPosition = SendMessage(_winampHandle, WM_WA_IPC, 0, IPC_GETLISTPOS);
        buf += "<br/>Current position: "+QString::number(playListPosition);
    }
    else
    {
        buf += "Stopped";
    }

    return true;
}
bool Winamp::onAction(const int& num)
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    switch(num)
    {
    case 1:
        SendMessage(_winampHandle, WM_COMMAND, WINAMP_VOLUMEUP, 0);
        break;
    case 2:
        SendMessage(_winampHandle, WM_COMMAND, WINAMP_VOLUMEDOWN, 0);
        break;
    case 3:
        SendMessage(_winampHandle, WM_COMMAND, 0, IPC_STARTPLAY);
        break;
    case 4:
        SendMessage(_winampHandle, WM_COMMAND, WINAMP_FFWD5S, 0);
        break;
    case 5:
        SendMessage(_winampHandle, WM_COMMAND, WINAMP_REW5S, 0);
        break;
    case 6:
          break;
    case 7:
        break;
    case 8:
        break;
    }

    return true;
}

bool Winamp::onPlay()
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    SendMessage(_winampHandle, WM_COMMAND, 40046, 0);

    return true;
}

bool Winamp::onStop()
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    SendMessage(_winampHandle, WM_COMMAND, WINAMP_BUTTON4, 0);

    return true;
}

bool Winamp::onNext()
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    SendMessage(_winampHandle, WM_COMMAND, WINAMP_BUTTON5, 0);

    return true;
}

bool Winamp::onPrev()
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    SendMessage(_winampHandle, WM_COMMAND, WINAMP_BUTTON1, 0);

    return true;
}

bool Winamp::onBack()
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    SendMessage(_winampHandle, WM_COMMAND, WINAMP_REW5S, 0);

    return true;
}

bool Winamp::onForw()
{
    if (!_winampHandle)
    {
        findWindow();
        if (!_winampHandle)
        {
            return false;
        }
    }

    SendMessage(_winampHandle, WM_COMMAND, WINAMP_FFWD5S, 0);

    return true;
}

void Winamp::findWindow()
{
    if (0 != _instrumentsCollection)
    {
        _winampHandle = _instrumentsCollection->getWindow(QString(""), QString("Winamp v1.x"));
    }
}

extern "C" __declspec(dllexport) PluginBase *getInstance()
{
    static PluginBase *inst = new Winamp();

    return inst;
}
