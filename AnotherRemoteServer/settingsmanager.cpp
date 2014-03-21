#include "settingsmanager.h"

SettingsManager * SettingsManager::_classInst = 0;

SettingsManager::SettingsManager() :
    _settingsHandler("AnotherRemoteServer.ini", QSettings::IniFormat)
  , _getSetMutex(QMutex::NonRecursive)
{
}

SettingsManager *SettingsManager::instance()
{
    static QMutex mtx(QMutex::NonRecursive);

    if (SettingsManager::_classInst == 0)
    {
        mtx.lock();
        if (SettingsManager::_classInst == 0)
        {
            _classInst = new SettingsManager;
        }
        mtx.unlock();
    }

    return _classInst;
}

void SettingsManager::drop()
{
    static QMutex mtx(QMutex::NonRecursive);

    mtx.lock();
    if (_classInst != 0)
    {
        delete _classInst;
        _classInst = 0;
    }
    mtx.unlock();
}

QHostAddress SettingsManager::getIP()
{
    _getSetMutex.lock();
    QString qsAddr =_settingsHandler.value("BindIP", "0.0.0.0").toString();
    QHostAddress qhAddr(qsAddr);
    _getSetMutex.unlock();
    return qhAddr;
}

unsigned short SettingsManager::getPort()
{
    _getSetMutex.lock();
    QString qsPort = _settingsHandler.value("BindPort", "4655").toString();

    bool ok = true;
    unsigned short port = qsPort.toShort();
    _getSetMutex.unlock();
    if (ok == true)
    {
        return port;
    }
    else
    {
        return 0;
    }
}

QString SettingsManager::getLastPlugin()
{
    _getSetMutex.lock();
    QString lPlugin = _settingsHandler.value("LastLoadedPlugin", "").toString();
    _getSetMutex.unlock();
    return lPlugin;
}

unsigned short SettingsManager::getPhonePort()
{
    _getSetMutex.lock();
    QString qsPort = _settingsHandler.value("PhonePort", "4655").toString();

    bool ok = true;
    unsigned short port = qsPort.toShort();
    _getSetMutex.unlock();
    if (ok == true)
    {
        return port;
    }
    else
    {
        return 0;
    }
}

void SettingsManager::saveIP(const QString &d)
{
    _getSetMutex.lock();
    _settingsHandler.setValue("BindIP", d);
    _getSetMutex.unlock();
}

void SettingsManager::savePort(const QString &d)
{
    _getSetMutex.lock();
    _settingsHandler.setValue("BindPort", d);
    _getSetMutex.unlock();
}

void SettingsManager::saveLastPlugin(const QString &name)
{
    _getSetMutex.lock();
    _settingsHandler.setValue("LastLoadedPlugin", name);
    _getSetMutex.unlock();
}

void SettingsManager::savePhonePort(const QString& p)
{
    _getSetMutex.lock();
    _settingsHandler.setValue("PhonePort", p);
    _getSetMutex.unlock();
}
