#include "doorphone.h"

void DoorPhone::runThreads()
{
    connect(&class1, &ThreadClass1::sendData, &tcp, &TCPHandler::slotToTCP); // send to TCP
    connect(&class1Thread, &QThread::started, &class1, &ThreadClass1::run);

    connect(&tcp, &TCPHandler::signalLog, this, &DoorPhone::toLog); // LOG
    connect(&tcpThread, &QThread::started, &tcp, &TCPHandler::run); // Start TCP Thread

    connect(&phone, &PhoneHandler::signalLog, this, &DoorPhone::toLog);
    connect(&phoneThread, &QThread::started, &phone, &PhoneHandler::start);

    // commands temp signals
    connect(this, &DoorPhone::signalCallStart, &tcp, &TCPHandler::slotCallStart);
    connect(this, &DoorPhone::signalCallStop, &tcp, &TCPHandler::slotCallStop);


    // Direct signals between threads
    connect(&tcp, &TCPHandler::signalStartPhone, &phone, &PhoneHandler::slotStartPhone);
    connect(&tcp, &TCPHandler::signalStopPhone, &phone, &PhoneHandler::slotStopPhone);

    class1.moveToThread(&class1Thread);
    class1Thread.start();

    tcp.moveToThread(&tcpThread);
    tcpThread.start();

    phone.moveToThread(&phoneThread);
    phoneThread.start();

    runTest();
}

void DoorPhone::toLog(QString _log)
{
    qDebug() << _log;
}

void DoorPhone::stopCalling()
{
    emit signalCallStop();
}

void DoorPhone::runTest()
{
    emit signalCallStart();
    QTimer::singleShot(10000, this, &DoorPhone::stopCalling);
}

DoorPhone::DoorPhone(QObject *parent)
    : QObject{parent}
{
    qDebug() << "Door phone started in thread: " << QThread::currentThreadId();
}
