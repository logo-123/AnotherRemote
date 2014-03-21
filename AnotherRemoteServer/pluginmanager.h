#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QLibrary>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QVector>
#include <QSysInfo>
#include <QtGlobal>
#include "PluginBase.hpp"
#include "InstrumentsCollectionInterface.h"

#if defined(Q_OS_WIN32)
#   define PLATFORM_PATH "\\plugins\\"
#   define PLATFORM_LIB  "dll"
#elif defined(Q_OS_LINUX)
#   define PLATFORM_PATH "/plugins"
#   define PLATFORM_LIB  "so"
#else
#   error "Unsupported OS detected. Windows or Linux valid only"
#endif

class PluginManager
{
public:
    PluginManager(InstrumentsCollectionInterface *obj);
    ~PluginManager();
    struct FullPluginData
    {
        QString _name;
        QString _actButtonsLabels;
        QLibrary *_handle;
        PluginBase *_inst;

        FullPluginData();
        ~FullPluginData();
    };

    enum buttons {Action1, Action2, Action3, Action4, Action5, Action6,
                  Action7, Action8, Play, Stop, Next, Prevoiuse, Back,
                  Forward, VolUp, VolDown, Mute, Hibernate, PowerOff, Sleep};

    bool getPluginName(QString &buf) const;
    bool getActionLabels(QString &buf) const;
    bool getPluginString(QString &buf) const;
    int getPluginCount() const;
    int getCurrentPlugin() const;
    bool selectPlugin(int num);
    bool selectPlugin(const QString &name);
    void selectNext();
    bool execButton(buttons exec);

private:
    QLibrary *_activeLib;
    int _pluginCount;
    int _currentPlugin;
    QVector<PluginManager::FullPluginData *> _pluginList;
    InstrumentsCollectionInterface *_icPointer;

    bool checkLibrary(const QString &libPath);
    void saveLast();
    void onLoad();
    void sendEvent(PluginBase::eventType e);
    void findPlugins(const QString& folder);
};

#endif // PLUGINMANAGER_H
