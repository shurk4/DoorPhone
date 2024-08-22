#include "doorphone.h"

void DoorPhone::runThreads()
{
    connect(&class1, &ThreadClass1::sendData, &tcp, &TCPClass::slotSendData); // send to TCP
    connect(&class1Thread, &QThread::started, &class1, &ThreadClass1::run);

    connect(&tcp, &TCPClass::signalLog, this, &DoorPhone::toLog); // LOG
    connect(&tcpThread, &QThread::started, &tcp, &TCPClass::run); // Start TCP Thread

    // commands temp signals
    connect(this, &DoorPhone::callStart, &tcp, &TCPClass::slotCallStart);
    connect(this, &DoorPhone::callStop, &tcp, &TCPClass::slotCallStop);

    class1.moveToThread(&class1Thread);
    class1Thread.start();

    tcp.moveToThread(&tcpThread);
    tcpThread.start();

    runTest();
}

void DoorPhone::toLog(QString _log)
{
    qDebug() << _log;
}

void DoorPhone::stopCalling()
{
    emit callStop();
}

void DoorPhone::runTest()
{
    emit callStart();
    QTimer::singleShot(10000, this, &DoorPhone::stopCalling);
}

DoorPhone::DoorPhone(QObject *parent)
    : QObject{parent}
{
    qDebug() << "Door phone started in thread: " << QThread::currentThreadId();
}
