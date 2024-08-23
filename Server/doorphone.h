#ifndef DOORPHONE_H
#define DOORPHONE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "tcpHandler.h"
#include "threadclass1.h"
#include "phoneHandler.h"

class DoorPhone : public QObject
{
    Q_OBJECT

    ThreadClass1 class1;
    QThread class1Thread;

    TCPHandler tcp;
    QThread tcpThread;

    PhoneHandler phone;
    QThread phoneThread;

    void runTest();

public:
    explicit DoorPhone(QObject *parent = nullptr);
    void runThreads();

signals:
    void signalCallStart(); // temp
    void signalCallStop(); // temp
    void signalPhoneStart();
    void signalPhoneStop();

public slots:
    void toLog(QString _log);

    void stopCalling(); // temp

};

#endif // DOORPHONE_H
