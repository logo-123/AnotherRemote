#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "powercontrol.h"
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostAddress>
#include <QList>
#include <QMessageBox>
#include <QFile>
#include <QIcon>
#include <QtGlobal>

#ifdef Q_OS_LINUX
#undef signals
extern "C"
{
    #include <libappindicator/app-indicator.h>
    #include <libnotify/notify.h>
    #include <gtk/gtk.h>
}
#define signals public
extern "C"
{
void onShow(GtkMenu*, gpointer);
void onQuit(GtkMenu*, gpointer);
}

void onShow(GtkMenu* menu, gpointer data)
{
    Q_UNUSED(menu);
    MainWindow* self = static_cast<MainWindow*>(data);
    self->iconClicked(QSystemTrayIcon::DoubleClick);
}

void onQuit(GtkMenu* menu, gpointer data)
{
    Q_UNUSED(menu);
    MainWindow* self = static_cast<MainWindow*>(data);
    self->exitAction();
}
#endif

static const QStringList htmlActions = (QStringList() << "VolUp" << "Mute" << "VolDown" <<
                                        "Act1" << "Act2" << "Act3" << "Act4" << "Act5" <<
                                        "Act6" << "Act7" << "Act8" << "Back" << "Play" <<
                                        "Forw" << "Prev" << "Stop" << "Next" <<
                                        "SwitchPlugin" << "Sleep" << "PowerOff" <<
                                        "Hibernate" << "MouseLButton" << "MouseRButton" <<
                                        "MouseUp" << "MouseLeft" << "MouseDown" <<
                                        "MouseRight" << "MouseLDown" << "MouseLUp" <<
                                        "MouseRDown" << "MouseRUp" << "GetPluginStr" <<
                                        "LoadButtonLabels" << "PING" << "TEST");



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _intValidator(0)
    , _iniHandler(0)
    , _lastIP()
    , _lastPort(0)
    , _sysTray(this)
    , _trayMenu(0)
    , _exitAction(0)
    , _settingsAction(0)
    , _soundManager()
    , _httpServer(this)
    , _sendBuffer("")
    , _ic(new InstrumentsCollection())
    , _pluginsHandler(_ic)
    , _lastPhonePort(0)
    , _phoneSocket(this)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()^Qt::WindowMaximizeButtonHint);
    setAttribute(Qt::WA_QuitOnClose);
    createTrayIcon();

    _iniHandler = SettingsManager::instance();
    _lastPort = _iniHandler->getPort();
    _lastIP = _iniHandler->getIP();
    _lastPhonePort = _iniHandler->getPhonePort();

    if (_ic)
    {
        _ic->setMouseStep(20);
    }

    _intValidator = new QIntValidator(0, 65535, this);
    ui->lineEdit->setValidator(_intValidator);
    ui->lineEdit_2->setValidator(_intValidator);

    QList<QNetworkInterface> ifaceList = QNetworkInterface::allInterfaces();
    for (int i = 0; i < ifaceList.size(); ++i)
    {
        if (ifaceList[i].addressEntries().size() > 0)
        {
            QString ips("");
            if (ifaceList[i].addressEntries()[0].ip().
                    toString().contains(QRegExp("[0-9]{1,3}(.[0-9]{1,3}){3,3}")))
            {
                ips += ifaceList[i].addressEntries()[0].ip().toString();
                ui->comboBox->addItem(ips + " - " + ifaceList[i].humanReadableName(),
                              QVariant(ifaceList[i].addressEntries()[0].ip().toString()));
                if (_lastIP == ifaceList[i].addressEntries()[0].ip())
                {
                    ui->comboBox->setCurrentIndex(ui->comboBox->count()-1);
                }
            }
        }
    }

    ui->lineEdit->setText(QString::number(_lastPort));
    ui->lineEdit_2->setText(QString::number(_lastPhonePort));;
    connect(&_httpServer, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
                this, SLOT(handle(QHttpRequest*, QHttpResponse*)));
    _httpServer.listen(_lastIP, _lastPort);
    _phoneSocket.bind(_lastIP, _lastPhonePort);
    connect(&_phoneSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    hide();
}

MainWindow::~MainWindow()
{
    dumpSettings();

    SettingsManager::drop();

    if (_intValidator)
    {
        delete _intValidator;
    }

    if (_exitAction)
    {
        delete _exitAction;
    }

    if (_settingsAction)
    {
        delete _settingsAction;
    }

    if (_trayMenu)
    {
        delete _trayMenu;
    }
    _httpServer.close();
    _sysTray.hide();
    delete ui;
}

void MainWindow::handle(QHttpRequest *req, QHttpResponse *res)
{
    if (req->path() == QLatin1String("/"))
    {
        showPage(res);
    }
    else if (req->path() == QLatin1String("/button"))
    {
        QString url(req->url().toString());
        substructParameters(url);
        res->setHeader("Content-type", "text/plain; charset=utf-8");
        res->setHeader("Cache-Control", "no-store, no-cache, must-revalidate");
        res->setHeader("Cache-Control", "post-check=0, pre-check=0");
        clientData(url, res);
    }
    else if(req->path() == QLatin1String("/mouse"))
    {
        QString url(req->url().toString());
        substructParameters(url);
        res->setHeader("Content-type", "text/plain; charset=utf-8");
        res->setHeader("Cache-Control", "no-store, no-cache, must-revalidate");
        res->setHeader("Cache-Control", "post-check=0, pre-check=0");
        handleMouseMoveDistance(url, res);
    }
    else
    {
        QString err(ERR_PAGE);
        res->setHeader("Content-Length", QString::number(err.length()));
        res->writeHead(404);
        res->write(err);
        res->end();
    }
}

bool MainWindow::substructParameters(QString &url)
{
    QStringList list(url.split("?"));

    if (list.size() == 2)
    {
        url = list[1];
        return true;
    }

    return false;
}

void MainWindow::showPage(QHttpResponse *res)
{
    res->setHeader("Content-Type", "text/html");
    QFile file("page.html");
    if (file.open(QIODevice::ReadOnly))
    {
        QString pageContent(file.readAll());
        QString actionLabels("");
        QStringList buttonNames;

        if (_pluginsHandler.getActionLabels(actionLabels))
        {
            buttonNames = actionLabels.split(";");
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                buttonNames.push_back(QString("Plugin error"));
            }
        }

        for (int i = 0; i < 8; ++i)
        {
            pageContent.replace("%"+QString::number(i+1), buttonNames[i]);
        }

        if (_pluginsHandler.getPluginName(actionLabels))
        {
            pageContent.replace("%9", actionLabels);
        }
        else
        {
            pageContent.replace("%9", "Plugin error");
        }
        res->setHeader("Content-Length", QString::number(pageContent.length()));
        res->writeHead(200);
        res->write(pageContent);
        pageContent.clear();
        file.close();
    }
    else
    {
        QString err(ERR_PAGE);
        res->setHeader("Content-Length", QString::number(err.length()));
        res->writeHead(404);
        res->write(err);
    }
    res->end();
}

void MainWindow::dumpSettings()
{
    int curSel = ui->comboBox->currentIndex();
    _iniHandler->saveIP(ui->comboBox->itemData(curSel).toString());
    _iniHandler->savePort(ui->lineEdit->text());
    _iniHandler->savePhonePort(ui->lineEdit_2->text());
}

void MainWindow::iconClicked(QSystemTrayIcon::ActivationReason act)
{
    if (act == QSystemTrayIcon::DoubleClick)
    {
        if (isHidden())
        {
            raise();
            showNormal();
        }
    }
}

void MainWindow::createTrayIcon()
{
#ifdef Q_OS_LINUX
    QString desktop;
    bool isUnity;

    desktop = getenv("XDG_CURRENT_DESKTOP");
    isUnity = (desktop.toLower() == "unity");
    if(isUnity) //only use this in unity
    {
        AppIndicator *indicator;
        GtkWidget *menu;
        GtkWidget *showItem;
        GtkWidget *quitItem;

        menu = gtk_menu_new();

        //show Item
        showItem = gtk_menu_item_new_with_label("Settings");
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), showItem);
        g_signal_connect(showItem, "activate", G_CALLBACK(onShow), this);
        gtk_widget_show(showItem);

        //quit item
        quitItem = gtk_menu_item_new_with_label("Quit");
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), quitItem);
        g_signal_connect(quitItem, "activate", G_CALLBACK(onQuit), this);
        gtk_widget_show(quitItem);

        indicator = app_indicator_new("AnotherRemoteServer", "AnotherRemoteServer",
                                              APP_INDICATOR_CATEGORY_OTHER);

        app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
        app_indicator_set_menu(indicator, GTK_MENU(menu));
        app_indicator_set_icon(indicator, "/usr/share/icons/ubuntu-mono-light/status/24/AnotherRemoteServer.png");
    }
    else //other DE's and OS's
    {
#endif
    QIcon appIcon(":/app_image.png");
    _trayMenu = new QMenu(this);
    _exitAction = new QAction("Exit", this);
    connect(_exitAction, SIGNAL(triggered()), SLOT(exitAction()));
    _settingsAction = new QAction("Settings", this);
    connect(_settingsAction, SIGNAL(triggered()), SLOT(settingsAction()));
    _trayMenu->addAction(_settingsAction);
    _trayMenu->addSeparator();
    _trayMenu->addAction(_exitAction);
    _sysTray.setContextMenu(_trayMenu);
    _sysTray.setToolTip("AnotherRemoteServer");
    _sysTray.setIcon(appIcon);
    setWindowIcon(appIcon);
    _sysTray.show();
    connect(&_sysTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));
#ifdef Q_OS_LINUX
    }
#endif
}

void MainWindow::exitAction()
{
    close();
}

void MainWindow::settingsAction()
{
    if (isHidden())
    {
        raise();
        showNormal();
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange)
    {
        if (isMinimized())
        {
            hide();
            e->ignore();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    hide();
}

void MainWindow::on_pushButton_clicked()
{
    dumpSettings();
    hide();
}

void MainWindow::clientData(const QString &data, QHttpResponse *res)
{
    bool ret = true;
    QString txtBuf("");

    switch (htmlActions.indexOf(data))
    {
    case 0: // Volume up
        _pluginsHandler.execButton(PluginManager::VolUp);
        ret = _soundManager.IncreaseVolume();
        break;
    case 1: // Volume mute
        _pluginsHandler.execButton(PluginManager::Mute);
        ret = _soundManager.ToggleMute();
        break;
    case 2: // Volume down
        _pluginsHandler.execButton(PluginManager::VolDown);
        ret = _soundManager.DecreaseVolume();
        break;
    case 3: // Action 1
        ret = _pluginsHandler.execButton(PluginManager::Action1);
        break;
    case 4: // Action 2
        ret = _pluginsHandler.execButton(PluginManager::Action2);
        break;
    case 5: // Action 3
        ret = _pluginsHandler.execButton(PluginManager::Action3);
        break;
    case 6: // Action 4
        ret = _pluginsHandler.execButton(PluginManager::Action4);
        break;
    case 7: // Action 5
        ret = _pluginsHandler.execButton(PluginManager::Action5);
        break;
    case 8: // Action 6
        ret = _pluginsHandler.execButton(PluginManager::Action6);
        break;
    case 9: // Action 7
        ret = _pluginsHandler.execButton(PluginManager::Action7);
        break;
    case 10: // Action 8
        ret = _pluginsHandler.execButton(PluginManager::Action8);
        break;
    case 11: // Backward
        ret = _pluginsHandler.execButton(PluginManager::Back);
        break;
    case 12: // Play
        ret = _pluginsHandler.execButton(PluginManager::Play);
        break;
    case 13: // Forward
        ret = _pluginsHandler.execButton(PluginManager::Forward);
        break;
    case 14: // Previous
        ret = _pluginsHandler.execButton(PluginManager::Prevoiuse);
        break;
    case 15: // Stop
        ret = _pluginsHandler.execButton(PluginManager::Stop);
        break;
    case 16: // Next
        ret = _pluginsHandler.execButton(PluginManager::Next);
        break;
    case 17: // Next plugin
        _pluginsHandler.selectNext();
        ret = packLabels(txtBuf);
        break;
    case 18: // Sleep
        makeResponse(res, true, txtBuf);
        _pluginsHandler.execButton(PluginManager::Sleep);
        PowerControl::setPowerState(PowerControl::SLEEP);
        res = 0;
        break;
    case 19: // Power off
        makeResponse(res, true, txtBuf);
        _pluginsHandler.execButton(PluginManager::PowerOff);
        PowerControl::setPowerState(PowerControl::POWEROFF);
        res = 0;
        break;
    case 20: // Hibernate
        makeResponse(res, true, txtBuf);
        _pluginsHandler.execButton(PluginManager::Hibernate);
        PowerControl::setPowerState(PowerControl::HIBERNATE);
        res = 0;
        break;
    case 21: // Left click
        if (_ic)
        {
            ret = _ic->mouseClick(InstrumentsCollection::MLeftButton);
        }
        break;
    case 22: // Right click
        if (_ic)
        {
            ret = _ic->mouseClick(InstrumentsCollection::MRightButton);
        }
        break;
    case 23: // Up
        if (_ic)
        {
            ret = _ic->mouseMoveStep(InstrumentsCollection::MStepUp);
        }
        break;
    case 24: // Left
        if (_ic)
        {
            ret = _ic->mouseMoveStep(InstrumentsCollection::MStepLeft);
        }
        break;
    case 25: // Down
        if (_ic)
        {
            ret = _ic->mouseMoveStep(InstrumentsCollection::MStepDown);
        }
        break;
    case 26: // Right
        if (_ic)
        {
            ret = _ic->mouseMoveStep(InstrumentsCollection::MStepRight);
        }
        break;
    case 27: // Left down
        if (_ic)
        {
            ret = _ic->mouseButtonEvent(InstrumentsCollection::MLeftButton, true);
        }
        break;
    case 28: // Left up
        if (_ic)
        {
            ret = _ic->mouseButtonEvent(InstrumentsCollection::MLeftButton, false);
        }
        break;
    case 29: // Right down
        if (_ic)
        {
            ret = _ic->mouseButtonEvent(InstrumentsCollection::MRightButton, true);
        }
        break;
    case 30: // Right up
        if (_ic)
        {
            ret = _ic->mouseButtonEvent(InstrumentsCollection::MRightButton, false);
        }
        break;
    case 31: // Plugin str
        ret = _pluginsHandler.getPluginString(txtBuf);
        break;
    case 32: // Load button labels
        ret = packLabels(txtBuf);
        break;
    case 33:
        txtBuf = "PONG";
        res = 0;
        break;
    case 34:
        txtBuf = "TEST-OK";
        break;
    default:
        break;
    }

    makeResponse(res, ret, txtBuf);
}

void MainWindow::makeResponse(QHttpResponse *res, bool execState, QString &text)
{
    if (0 == res)
    {
        return;
    }
    if (execState)
    {
        res->setHeader("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0");
        res->setHeader("Pragma", "no-cache");
        res->setHeader("Expires", "Tue, 01 Jan 1980 1:00:00 GMT");
        res->setHeader("Content-Type", "text/plain; charset=utf-8");
        if (!text.isEmpty())
        {
            text = text.toUtf8();
            res->setHeader("Content-Length", QString::number(text.length()));
            res->writeHead(200);
            res->write(text);
        }
        else
        {
            res->setHeader("Content-Length", "0");
            res->writeHead(200);
        }
    }
    else
    {
        res->setHeader("Content-Length", "0");
        res->writeHead(500);
        res->end();
    }
}

void MainWindow::handleMouseMoveDistance(const QString &data, QHttpResponse *res)
{
    QStringList paramList = data.split(":", QString::SkipEmptyParts);

    if (paramList.size() == 4)
    {
        qDebug() << "Mouse delta " << paramList[0] << ": " << paramList[1].toInt() << " " << paramList[2] << ": " << paramList[3].toInt();

        if (_ic)
        {
            _ic->mouseMoveRelative(paramList[1].toInt(), paramList[3].toInt());
        }
    }

    res->setHeader("Content-Length", "0");
    res->writeHead(200);
}

bool MainWindow::packLabels(QString& buf)
{
    QString txtBuf2;

    buf = "newLabels;";
    if (true == _pluginsHandler.getPluginName(txtBuf2))
    {
        buf += txtBuf2;
        if (true == _pluginsHandler.getActionLabels(txtBuf2))
        {
            buf += ";"+txtBuf2;
        }
        else
        {
            buf = "Err";
            return false;
        }
    }
    else
    {
        buf = "Err";
        return false;
    }

    return true;
}

void MainWindow::readPendingDatagrams()
{
    char* buf = new char[1500] ();
    QHostAddress senderAddr;
    unsigned short senderPort = 0;

    _phoneSocket.readDatagram (buf, 1500, &senderAddr, &senderPort);

    if ((0 != buf) && (0 != strlen(buf)))
    {
        if (0 != strcmp(buf, "PING"))
        {
            qDebug() << "Command from phone " << buf;
        }

        if (0 == strcmp(buf, "PLUGIN_STRING"))
        {
            QString buf;

            if (true == _pluginsHandler.getPluginString(buf))
            {
                _phoneSocket.writeDatagram(buf.toLatin1().data(), buf.length(), senderAddr, senderPort);
            }
        }
        else if (0 == strcmp(buf, "PING"))
        {
            QString buf = "PONG";
            _phoneSocket.writeDatagram(buf.toLatin1().data(), buf.length(), senderAddr, senderPort);
        }
        else if (0 == strcmp(buf, "CHECK_SERVER"))
        {
            QString buf = "SERVER_OK";
            _phoneSocket.writeDatagram(buf.toLatin1().data(), buf.length(), senderAddr, senderPort);
        }
        else if (0 == strcmp(buf, "LBUTTONDOWN"))
        {
            if (0 != _ic)
            {
                //_ic->mouseButtonEvent(InstrumentsCollection::MLeftButton, false);
            }
        }
        else if (0 == strcmp(buf, "LBUTTONUP"))
        {
            if (0 != _ic)
            {
                //_ic->mouseButtonEvent(InstrumentsCollection::MLeftButton, true);
            }
        }
        else if (0 == strcmp(buf, "RBUTTONDOWN"))
        {
            if (0 != _ic)
            {
                //_ic->mouseButtonEvent(InstrumentsCollection::MRightButton, false);
            }
        }
        else if (0 == strcmp(buf, "RBUTTONUP"))
        {
            if (0 != _ic)
            {
                //_ic->mouseButtonEvent(InstrumentsCollection::MRightButton, true);
            }
        }
        else if (0 == strcmp(buf, "BUTTONS_LABEL"))
        {
            QString buf;
            if (true == packLabels(buf))
            {
                 _phoneSocket.writeDatagram(buf.toLatin1().data(), buf.length(), senderAddr, senderPort);
            }
        }
        else if (0 == strcmp(buf, "VOLUME_UP"))
        {
            _pluginsHandler.execButton(PluginManager::VolUp);
            _soundManager.IncreaseVolume();
        }
        else if (0 == strcmp(buf, "MUTE"))
        {
            _pluginsHandler.execButton(PluginManager::Mute);
            _soundManager.ToggleMute();
        }
        else if (0 == strcmp(buf, "VOLUME_DOWN"))
        {
            _pluginsHandler.execButton(PluginManager::VolDown);
            _soundManager.DecreaseVolume();
        }
        else if (0 == strcmp(buf, "SWITCH_PLUGIN"))
        {
            _pluginsHandler.selectNext();
            QString buf;
            if (true == packLabels(buf))
            {
                 _phoneSocket.writeDatagram(buf.toLatin1().data(), buf.length(), senderAddr, senderPort);
            }
        }
        else if (0 == strcmp(buf, "NEXT"))
        {
            _pluginsHandler.execButton(PluginManager::Next);
        }
        else if (0 == strcmp(buf, "PLAY"))
        {
            _pluginsHandler.execButton(PluginManager::Play);
        }
        else if (0 == strcmp(buf, "PREVIOUS"))
        {
            _pluginsHandler.execButton(PluginManager::Prevoiuse);
        }
        else if (0 == strcmp(buf, "BACKWARD"))
        {
            _pluginsHandler.execButton(PluginManager::Back);
        }
        else if (0 == strcmp(buf, "FORWARD"))
        {
            _pluginsHandler.execButton(PluginManager::Forward);
        }
        else if (0 == strcmp(buf, "STOP"))
        {
            _pluginsHandler.execButton(PluginManager::Stop);
        }
        else if (0 == strcmp(buf, "ACTION_1"))
        {
            _pluginsHandler.execButton(PluginManager::Action1);
        }
        else if (0 == strcmp(buf, "ACTION_2"))
        {
            _pluginsHandler.execButton(PluginManager::Action2);
        }
        else if (0 == strcmp(buf, "ACTION_3"))
        {
            _pluginsHandler.execButton(PluginManager::Action3);
        }
        else if (0 == strcmp(buf, "ACTION_4"))
        {
            _pluginsHandler.execButton(PluginManager::Action4);
        }
        else if (0 == strcmp(buf, "ACTION_5"))
        {
            _pluginsHandler.execButton(PluginManager::Action5);
        }
        else if (0 == strcmp(buf, "ACTION_6"))
        {
            _pluginsHandler.execButton(PluginManager::Action6);
        }
        else if (0 == strcmp(buf, "ACTION_7"))
        {
            _pluginsHandler.execButton(PluginManager::Action7);
        }
        else if (0 == strcmp(buf, "ACTION_8"))
        {
            _pluginsHandler.execButton(PluginManager::Action8);
        }
        else if (0 == strcmp(buf, "POWER_OFF"))
        {
            _pluginsHandler.execButton(PluginManager::PowerOff);
            PowerControl::setPowerState(PowerControl::POWEROFF);
        }
        else if (0 == strcmp(buf, "HIBERNATE"))
        {
            _pluginsHandler.execButton(PluginManager::Hibernate);
            PowerControl::setPowerState(PowerControl::HIBERNATE);
        }
        else if (0 == strcmp(buf, "SLEEP"))
        {
            _pluginsHandler.execButton(PluginManager::Sleep);
            PowerControl::setPowerState(PowerControl::SLEEP);
        }
        else if (0 == strncmp(buf, "MMOVE: ", 7))
        {
            QString d((buf + 7));

            QStringList dXdY(d.split(" "));
            if (2 == dXdY.size())
            {
                bool ok = false;

                int dX = dXdY[0].toInt(&ok);
                if (true == ok)
                {
                    int dY = dXdY[1].toInt(&ok);
                    if (true == ok)
                    {
                        if (0 != _ic)
                        {
                            qDebug() << "Relative move for X: " << dX << " Y: " << dY;
                            _ic->mouseMoveRelative(dX, dY);
                        }
                    }
                }
            }
        }
        else if (0 == strcmp(buf, "RBUTTONCLICK"))
        {
            if (0 != _ic)
            {
                //_ic->mouseClick(InstrumentsCollection::MRightButton);
            }
        }
        else if (0 == strcmp(buf, "LBUTTONCLICK"))
        {
            //_ic->mouseClick(InstrumentsCollection::MLeftButton);
        }
        else if (0 == strcmp(buf, "MSTEPUP"))
        {
            _ic->mouseMoveStep(InstrumentsCollection::MStepUp);
        }
        else if (0 == strcmp(buf, "MSTEPDOWN"))
        {
            _ic->mouseMoveStep(InstrumentsCollection::MStepDown);
        }
        else if (0 == strcmp(buf, "MSTEPRIGHT"))
        {
            _ic->mouseClick(InstrumentsCollection::MStepRight);
        }
        else if (0 == strcmp(buf, "MSTEPLEFT"))
        {
            _ic->mouseClick(InstrumentsCollection::MStepLeft);
        }
        else
        {
            qDebug() << "Unknown command: " << buf;
        }
    }

    delete [] buf;
}
