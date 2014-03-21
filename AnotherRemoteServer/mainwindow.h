#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QtNetwork/QUdpSocket>
#include "httpserver/qhttpserver.h"
#include "httpserver/qhttprequest.h"
#include "httpserver/qhttpresponse.h"
#include "settingsmanager.h"
#include "pluginmanager.h"
#include "Audio/SoundManager.h"
#include "InstrumentsCollection.h"

#define ERR_PAGE "<!doctype html><html><head><title>AnotherRemote</title>\
                  </head><body><font coolor='red' size='4'>Page not found</font></body></html>"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum {VOLUP = 0, MUTE, VOLDOWN, SLEEP, HIBERNATE, POWEROFF, ACTION1, ACTION2, ACTION3,
          ACTION4, ACTION5, ACTION6, ACTION7, ACTION8, PREVIOUSE, BACK, STOP, PLAY, FORWARD,
          NEXT, NEXTPLUGIN, MOUSEUP, MOUSEDOWN, MLEFTDOWN, MLEFTUP, MRIGHTDOWN, MRIGHTUP,
          MUP, MDOWN, MRIGHT, MLEFT, PLUGINSTRING};

private slots:
    void settingsAction();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void handle(QHttpRequest *req, QHttpResponse *res);
    void readPendingDatagrams();

public slots:
    void iconClicked(QSystemTrayIcon::ActivationReason act);
    void exitAction();

private:
    Ui::MainWindow* ui;
    QIntValidator* _intValidator;
    SettingsManager* _iniHandler;
    QHostAddress _lastIP;
    unsigned short _lastPort;
    QSystemTrayIcon _sysTray;
    QMenu* _trayMenu;
    QAction* _exitAction,* _settingsAction;
    SoundManager _soundManager;
    QHttpServer _httpServer;
    QString _sendBuffer;
    InstrumentsCollectionInterface* _ic;
    PluginManager _pluginsHandler;
    unsigned short _lastPhonePort;
    QUdpSocket _phoneSocket;

    void dumpSettings();
    void createTrayIcon();
    void changeEvent(QEvent *e);
    void sendNameNButtonLabels();
    void sendPluginString();
    void clientData(const QString &data, QHttpResponse *res);
    void sendResponse(QHttpResponse *res);
    void makeResponse(QHttpResponse *res, bool execState, QString &text);
    void showPage(QHttpResponse *res);
    bool substructParameters(QString &url);
    void handleMouseMoveDistance(const QString &data, QHttpResponse *res);
    bool packLabels(QString& buf);
};

#endif // MAINWINDOW_H
