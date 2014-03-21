#ifndef PHILIPSREMOTE_H
#define PHILIPSREMOTE_H

#include "philipsremote_global.h"
#include "../AnotherRemoteServer/PluginBase.hpp"
#include <QtNetwork/QTcpSocket>

class PHILIPSREMOTESHARED_EXPORT PhilipsRemote
: public QObject
, public PluginBase

{
    Q_OBJECT
public:
    PhilipsRemote();
    ~PhilipsRemote();
    void eventReceived(const eventType& type);
    void onPluginSelected();
    bool getPluginString(QString& buf);
    bool onAction(const int& num);
    bool onPlay();
    bool onStop();
    bool onNext();
    bool onPrev();
    bool onBack();
    bool onForw();

public slots:
    void socketReply();

private:
    bool sendPostRequest(const QString &url, const QString &jsonRequest);
    void init();

    QTcpSocket mSocketHandler;
    QString mTVAddress;
    quint16 mTVPort;
};

#endif // PHILIPSREMOTE_H
