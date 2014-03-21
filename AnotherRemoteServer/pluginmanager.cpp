#include <QDebug>
#include "pluginmanager.h"
#include "PluginBase.hpp"
#include "settingsmanager.h"

PluginManager::FullPluginData::FullPluginData()
    : _name("")
    , _actButtonsLabels("")
    , _handle(0)
    , _inst(0)
{
}

PluginManager::FullPluginData::~FullPluginData()
{
    if (_inst)
    {
        delete _inst;
        _inst = 0;
    }
    if (_handle)
    {
        if(_handle->isLoaded())
        {
            _handle->unload();
        }
        delete _handle;
        _handle = 0;
    }
}

PluginManager::PluginManager(InstrumentsCollectionInterface *obj)
    : _pluginCount(0)
    , _currentPlugin(-1)
    , _pluginList()
    , _icPointer(obj)
{
    findPlugins(QDir::currentPath()+PLATFORM_PATH);

    if (_pluginCount > 0)
    {
        QString lPlugin = SettingsManager::instance()->getLastPlugin();
        if (lPlugin != "")
        {
            selectPlugin(lPlugin);
            if (_currentPlugin == -1)
            {
                _currentPlugin = 0;
                onLoad();
                saveLast();
            }
        }
        else
        {
            _currentPlugin = 0;
            onLoad();
            saveLast();
        }
    }
}

void PluginManager::findPlugins(const QString& folder)
{
    QDir filesListHandler(folder);
    filesListHandler.setFilter(QDir::NoDotAndDotDot|QDir::NoSymLinks|QDir::AllEntries);

    QFileInfoList files = filesListHandler.entryInfoList();

    qDebug() << files.size() << " files in " << folder;

    foreach(const QFileInfo& f, files)
    {
        QString path = f.absoluteFilePath();
        if (true == f.isDir())
        {
            findPlugins(path);
        }
        else
        {
            if (checkLibrary(path))
            {
                ++_pluginCount;
            }
        }
    }
}

PluginManager::~PluginManager()
{
    for (int i = 0; i < _pluginList.size(); ++i)
    {
        delete _pluginList[i];
    }

    _pluginList.clear();
}

bool PluginManager::checkLibrary(const QString &libPath)
{
    PluginManager::FullPluginData *fpd = new PluginManager::FullPluginData();
    qDebug() << "Loading: " << libPath;
    bool ret = false;

    if (0 == fpd)
    {
        qDebug() << "Unable to create FullPluginData object [" << libPath << "]";
        return false;
    }

    fpd->_handle = new QLibrary(libPath);
    if (0 == fpd->_handle)
    {
        qDebug() << "Unable to create QLibrary object [" << libPath << "]";
        return false;
    }

    if (false == fpd->_handle->load())
    {
        qDebug() << "Unable to load libary [" << libPath << "]";
        qDebug() << fpd->_handle->errorString();
        return false;
    }

    getInstType getInst = reinterpret_cast<getInstType> (fpd->_handle->resolve("getInstance"));
    if (0 == getInst)
    {
        qDebug() << "Unable to find getInstance [" << libPath << "]";
        return false;
    }

    PluginBase *pluginClass = getInst();
    if (0 == pluginClass)
    {
        qDebug() << "Unable to instanciate plugin class [" << libPath << "]";
        return false;
    }

    PluginBase::PluginDATA pd;
    pluginClass->getName(pd);
    if (pd.pluginName != "")
    {
        fpd->_name = pd.pluginName;
        fpd->_inst = pluginClass;
        fpd->_inst->setInstrumentsCollection(_icPointer);
        for (int i = 0; i < 7; ++i)
        {
            fpd->_actButtonsLabels.append(pd.actions[i]+";");
        }
        fpd->_actButtonsLabels.append(pd.actions[7]);
        if (fpd->_actButtonsLabels != "")
        {
            _pluginList.push_back(fpd);
            ret = true;
        }
    }

    if (!ret)
    {
        delete fpd->_handle;
        delete fpd;
    }

    return ret;
}

void PluginManager::selectNext()
{
    if (_pluginCount > 0)
    {
        ++_currentPlugin;

        if (_currentPlugin >= _pluginList.size())
        {
            _currentPlugin = 0;
        }

        saveLast();
        onLoad();
    }
}

bool PluginManager::selectPlugin(const QString &name)
{
    bool ret = false;

    if (_pluginCount > 0)
    {
        for(int i = 0; i <_pluginList.size(); ++i)
        {
            if (name == _pluginList[i]->_name)
            {
                _currentPlugin = i;
                saveLast();
                onLoad();
                ret = true;
                break;
            }
        }
    }

    return ret;
}

bool PluginManager::selectPlugin(int num)
{
    if (_pluginCount > 0)
    {
        if ((num >= 0) && (num < _pluginList.size()))
        {
            _currentPlugin = num;
            saveLast();
            onLoad();
            return true;
        }
    }

    return false;
}

int PluginManager::getCurrentPlugin() const
{
    return _currentPlugin;
}

int PluginManager::getPluginCount() const
{
    return _pluginList.size();
}

bool PluginManager::getActionLabels(QString &buf) const
{
    if (_pluginCount > 0)
    {
        if ((_currentPlugin >= 0) && (_currentPlugin < _pluginList.size()))
        {
            buf = _pluginList[_currentPlugin]->_actButtonsLabels;
            return true;
        }
    }

    return false;
}

bool PluginManager::getPluginString(QString &buf) const
{
    if (_pluginCount > 0)
    {
        if ((_currentPlugin >= 0) && (_currentPlugin < _pluginList.size()))
        {
            return _pluginList[_currentPlugin]->_inst->getPluginString(buf);
        }
    }

    return false;
}

bool PluginManager::getPluginName(QString &buf) const
{
    if (_pluginCount > 0)
    {
        if ((_currentPlugin >= 0) && (_currentPlugin < _pluginList.size()))
        {
            buf = _pluginList[_currentPlugin]->_name;
            return true;
        }
    }

    return false;
}

void PluginManager::saveLast()
{
    bool ok = false;
    QString tmp("");
    ok = getPluginName(tmp);

    if (ok)
    {
        SettingsManager::instance()->saveLastPlugin(tmp);
    }
}

void PluginManager::onLoad()
{
    if ((_currentPlugin < _pluginList.size()) && (_currentPlugin >= 0))
    {
        if (_pluginList[_currentPlugin]->_inst)
        {
            _pluginList[_currentPlugin]->_inst->onPluginSelected();
        }
    }
}

bool PluginManager::execButton(buttons exec)
{
    if ((_currentPlugin > _pluginList.size()) || (_currentPlugin < 0))
    {
        qDebug() << "PluginManager::execButton - invalid plugin number: " << _currentPlugin;
        return false;
    }

    if (!_pluginList[_currentPlugin]->_inst)
    {
        qDebug() << "PluginManager::execButton - plugin instance for " << _currentPlugin << " is NULL";
        return false;
    }

    bool ret = true;

    switch(exec)
    {
    case Action1:
        ret = _pluginList[_currentPlugin]->_inst->onAction(1);
        break;
    case Action2:
        ret = _pluginList[_currentPlugin]->_inst->onAction(2);
        break;
    case Action3:
        ret = _pluginList[_currentPlugin]->_inst->onAction(3);
        break;
    case Action4:
        ret = _pluginList[_currentPlugin]->_inst->onAction(4);
        break;
    case Action5:
        ret = _pluginList[_currentPlugin]->_inst->onAction(5);
        break;
    case Action6:
        ret = _pluginList[_currentPlugin]->_inst->onAction(6);
        break;
    case Action7:
        ret = _pluginList[_currentPlugin]->_inst->onAction(7);
        break;
    case Action8:
        ret = _pluginList[_currentPlugin]->_inst->onAction(8);
        break;
    case Play:
        ret = _pluginList[_currentPlugin]->_inst->onPlay();
        break;
    case Stop:
        ret = _pluginList[_currentPlugin]->_inst->onStop();
        break;
    case Next:
        ret = _pluginList[_currentPlugin]->_inst->onNext();
        break;
    case Prevoiuse:
        ret = _pluginList[_currentPlugin]->_inst->onPrev();
        break;
    case Back:
        ret = _pluginList[_currentPlugin]->_inst->onBack();
        break;
    case Forward:
        ret = _pluginList[_currentPlugin]->_inst->onForw();
        break;
    case VolUp:
        sendEvent(PluginBase::VolUp);
        break;
    case VolDown:
        sendEvent(PluginBase::VolDown);
        break;
    case Mute:
        sendEvent(PluginBase::Mute);
        break;
    case Hibernate:
        sendEvent(PluginBase::Hibernate);
        break;
    case Sleep:
        sendEvent(PluginBase::Sleep);
        break;
    case PowerOff:
        sendEvent(PluginBase::PowerOff);
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

void PluginManager::sendEvent(PluginBase::eventType e)
{
    for (int i = 0; i < _pluginList.size(); ++i)
    {
        if (0 != _pluginList[i]->_inst)
        {
            _pluginList[i]->_inst->eventReceived(e);
        }
    }
}
