#include "philipsremote.h"
#include <QtGlobal>
#include <QSettings>
#include <QDebug>
#if defined(Q_OS_LINUX)
#include <unistd.h>
#elif defined(Q_OS_WIN32)
#include <WinBase.h>
#endif

const char* const pName = "PhilipsRemote";
const char* const aName[8] = {"Option", "Back", "CursorUp", "Confirm",
                               "Home", "CursorLeft", "CursorDown", "CursorRight"};

PhilipsRemote::PhilipsRemote()
: QObject()
, PluginBase()
, mSocketHandler()
, mTVAddress()
, mTVPort(0)
{
    _myData.setName(QString(pName));

     for (unsigned int i = 0; i < 8; ++i)
     {
         _myData.setActionName(i, QString(aName[i]));
     }

     connect(&mSocketHandler, SIGNAL(readyRead()), SLOT(socketReply()));

     init();
}

PhilipsRemote::~PhilipsRemote()
{

}

void PhilipsRemote::init()
{
    QSettings* settings = new QSettings("PhilipsRemoteSettings.ini", QSettings::IniFormat, this);

    if (0 != settings)
    {
        mTVAddress = settings->value("tv_address", "0.0.0.0").toString();
        mTVPort = settings->value("tv_port", "0").toString().toShort();

        if ((QLatin1String("0.0.0.0") == mTVAddress) || (0 == mTVPort))
        {
            settings->setValue("tv_address", "10.1.4.6");
            settings->setValue("tv_port", "1925");
            mTVAddress = "10.1.4.6";
            mTVPort = 1925;
        }

        delete settings;
    }
}

void PhilipsRemote::socketReply()
{
   QByteArray data = mSocketHandler.readAll();

   qDebug() << "Received from tv:";
   qDebug() << data;
}

void PhilipsRemote::eventReceived(const eventType& type)
{
    qDebug() << "Get event: " << ((PluginBase::Hibernate==type)?"Hibernate":
        (PluginBase::PowerOff==type)?"PowerOff":(PluginBase::Sleep==type)?"Sleep":"Else");

    switch (type)
    {
    case PluginBase::PowerOff:
    case PluginBase::Sleep:
    case PluginBase::Hibernate:
        sendPostRequest("1/input/key", "{\"key\":\"Standby\"}");
        break;
    default:
        break;
    }
}

void PhilipsRemote::onPluginSelected()
{
    if (("" == mTVAddress) || (0 == mTVPort))
    {
        init();
    }
}

bool PhilipsRemote::getPluginString(QString& buf)
{
    buf.clear();
    return true;
}

bool PhilipsRemote::onAction(const int& num)
{
    bool ret = false;

    switch (num)
    {
    case 1:
        ret = sendPostRequest("1/input/key", "{\"key\":\"Options\"}");
        break;
    case 2:
        ret = sendPostRequest("1/input/key", "{\"key\":\"Back\"}");
        break;
    case 3:
        ret = sendPostRequest("1/input/key", "{\"key\":\"CursorUp\"}");
        break;
    case 4:
        ret = sendPostRequest("1/input/key", "{\"key\":\"Confirm\"}");
        break;
    case 5:
        ret = sendPostRequest("1/input/key", "{\"key\":\"Home\"}");
        break;
    case 6:
        ret = sendPostRequest("1/input/key", "{\"key\":\"CursorLeft\"}");
        break;
    case 7:
        ret = sendPostRequest("1/input/key", "{\"key\":\"CursorDown\"}");
        break;
    case 8:
        ret = sendPostRequest("1/input/key", "{\"key\":\"CursorRight\"}");
        break;
    default:
        break;
    }

    return ret;
}

bool PhilipsRemote::onPlay()
{
    return sendPostRequest("1/input/key", "{\"key\":\"PlayPause\"}");
}

bool PhilipsRemote::onStop()
{
    return sendPostRequest("1/input/key", "{\"key\":\"Stop\"}");
}

bool PhilipsRemote::onNext()
{
    return sendPostRequest("1/input/key", "{\"key\":\"Next\"}");
}

bool PhilipsRemote::onPrev()
{
    return sendPostRequest("1/input/key", "{\"key\":\"Previous\"}");
}

bool PhilipsRemote::onBack()
{
    return sendPostRequest("1/input/key", "{\"key\":\"Rewind\"}");
}

bool PhilipsRemote::onForw()
{
    return sendPostRequest("1/input/key", "{\"key\":\"FastForward\"}");
}

bool PhilipsRemote::sendPostRequest(const QString& url, const QString& jsonRequest)
{
    QByteArray data;
    bool ret = false;

    data.append("POST /"+url+" HTTP/1.1\r\n");
    data.append("Host: "+mTVAddress+":"+QString::number(mTVPort)+"\r\n");
    data.append("User-Agent: Mozilla/5.0 (Windows NT 6.2; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1\r\n");
    data.append("Accept: application/json, text/javascript, */*; q=0.01\r\n");
    data.append("Accept-Language: en-US,en;q=0.5\r\n");
    data.append("Accept-Encoding: gzip, deflate\r\n");
    data.append("Referer: /test/\r\n");
    data.append("Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n");
    data.append("Content-Length: "+QString::number(jsonRequest.length())+"\r\n");
    data.append("Connection: keep-alive\r\n");
    data.append("Pragma: no-cache\r\n");
    data.append("Cache-Control: no-cache\r\n");
    data.append("\r\n");
    data.append(jsonRequest);

    mSocketHandler.connectToHost(mTVAddress, mTVPort);

    qDebug() << "Sending to tv:";
    qDebug() << data;

    if (true == mSocketHandler.waitForConnected())
    {
       if (-1 != mSocketHandler.write(data))
       {
          ret = true;
       }

       mSocketHandler.disconnectFromHost();
    }

    return ret;
}

#if defined(Q_OS_LINUX)
extern "C" PluginBase* getInstance()
#elif defined(Q_OS_WIN32)
extern "C" __declspec(dllexport) PluginBase* getInstance()
#endif
{
    static PluginBase* inst = new PhilipsRemote();

    return inst;
}
