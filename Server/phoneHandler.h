#ifndef PHONEHANDLER_H
#define PHONEHANDLER_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QSettings>

#include "callplayer.h"
#include "udpphone.h"

class PhoneHandler : public QObject
{
    Q_OBJECT

    CallPlayer doorbell;
    QTimer callTimer;
    int callTimerTime = 10000;
    bool calling = false;

    UDPPhone phone;
    bool answered = false;

    void readSettings();

public:
    explicit PhoneHandler(QObject *parent = nullptr);

signals:
    void signalLog(QString);

public slots:
    void toLog(QString _log);
    void start();

    // door bell
    void slotStartCall();
    void slotStopCall();

    // phone
    void slotStartPhone();
    void slotStopPhone();

};

#endif // PHONEHANDLER_H
