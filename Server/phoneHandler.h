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
//    QThread doorbellThread;

    QTimer callTimer;
    int callTimerTime = 10000;

    UDPPhone phone;

    void readSettings();

public:
    explicit PhoneHandler(QObject *parent = nullptr);

signals:
    void signalLog(QString);
    void signalCallStart(bool);
    void signalCallStop();
    void signalStartCallTimer(int);
    void signalStopCallTimer();


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
