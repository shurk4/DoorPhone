#ifndef DOORPHONE_H
#define DOORPHONE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "pins.h"
#include "tcpHandler.h"
#include "threadclass1.h"
#include "phoneHandler.h"

class DoorPhone : public QObject
{
    Q_OBJECT

//    ThreadClass1 class1;
//    QThread class1Thread;

    Pins gpio;
    QThread pinsLookupThread;
    QVector<int>inputPins;
//    QVector<int>outputPins;

    int buttonCallPin = 11;
    int door1Pin = 12;
    int door2Pin = 14;
    void initGPIO();

    TCPHandler tcp;
    QThread tcpThread;

    PhoneHandler phone;
    QThread phoneThread;

    bool calling = false;
    bool answered = false;

//    void runTest();

public:
    explicit DoorPhone(QObject *parent = nullptr);
    void runThreads();

signals:
    void signalStopGPIO();

    void signalPhoneStart();
    void signalPhoneStop();

public slots:
    void toLog(QString _log);

    void stopCalling(); // connect to tcpHandler
    void callStopped(); // connect to callPlayer

    void pinStateChanged(int _pin, int _state); // connect to gpio

};

#endif // DOORPHONE_H
