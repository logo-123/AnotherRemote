#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QString>
#include <QtNetwork/QHostAddress>
#include <QMutex>

class SettingsManager
{
public:
    static SettingsManager *instance();
    static void drop();
    void saveIP(const QString& d);
    void savePort(const QString& d);
    void saveLastPlugin(const QString& name);
    void savePhonePort(const QString& p);
    QHostAddress getIP();
    unsigned short getPort();
    QString getLastPlugin();
    unsigned short getPhonePort();

private:
    SettingsManager();
    ~SettingsManager() {}
    SettingsManager(const SettingsManager&) {}

    static SettingsManager* _classInst;

    QSettings _settingsHandler;
    QMutex _getSetMutex;
};

#endif // SETTINGSMANAGER_H
