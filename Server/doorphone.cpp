#include "doorphone.h"

void DoorPhone::runThreads()
{
//    connect(&class1, &ThreadClass1::sendData, &tcp, &TCPHandler::slotToTCP); // send to TCP
//    connect(&class1Thread, &QThread::started, &class1, &ThreadClass1::run);

    connect(&tcp, &TCPHandler::signalLog, this, &DoorPhone::toLog); // LOG
    connect(&tcpThread, &QThread::started, &tcp, &TCPHandler::run); // Start TCP Thread

    connect(&phone, &PhoneHandler::signalLog, this, &DoorPhone::toLog);
    connect(&phoneThread, &QThread::started, &phone, &PhoneHandler::start);

    // commands temp signals
    connect(this, &DoorPhone::signalCallStart, &tcp, &TCPHandler::slotCallStart); // recive from phoneHandler
    connect(this, &DoorPhone::signalCallStart, &phone, &PhoneHandler::slotStartCall);
    connect(this, &DoorPhone::signalCallStop, &tcp, &TCPHandler::slotCallStop);
    connect(this, &DoorPhone::signalCallStop, this, &DoorPhone::callStopped);

    // Direct signals between threads
    connect(&tcp, &TCPHandler::signalStartPhone, &phone, &PhoneHandler::slotStartPhone);
    connect(&tcp, &TCPHandler::signalStopPhone, &phone, &PhoneHandler::slotStopPhone);

//    class1.moveToThread(&class1Thread);
//    class1Thread.start();

    tcp.moveToThread(&tcpThread);
    tcpThread.start();

    phone.moveToThread(&phoneThread);
//    phoneThread.start(QThread::TimeCriticalPriority);
    phoneThread.start();

    initGPIO();
}

void DoorPhone::toLog(QString _log)
{
    qDebug() << _log;
}

void DoorPhone::callStopped()
{
    calling = false;
}

void DoorPhone::stopCalling()
{
    emit signalCallStop();
}

void DoorPhone::pinStateChanged(int _pin, int _state)
{
    toLog("Slot pinStateChanged");
    if(_pin == buttonCallPin)
    {
        calling = true;
        toLog("Emitting start call signal");
        emit signalCallStart();
    }
}

void DoorPhone::initGPIO()
{
    toLog("Initialize GPIO");
    wiringPiSetup();

    pinMode(door1Pin, OUTPUT);
    digitalWrite(door1Pin, LOW);
    pinMode(door2Pin, OUTPUT);
    digitalWrite(door2Pin, LOW);

    inputPins.push_back(buttonCallPin);
    gpio.setPins(inputPins);

    connect(this, &DoorPhone::signalStopGPIO, &gpio, &Pins::stopLoop);
    connect(&gpio, &Pins::pinStateChanged, this, &DoorPhone::pinStateChanged);
    connect(&gpio, &Pins::signalLog, this, &DoorPhone::toLog);
    connect(&pinsLookupThread, &QThread::started, &gpio, &Pins::run);
    connect(&gpio, &Pins::finished, &pinsLookupThread, &QThread::terminate);

    gpio.moveToThread(&pinsLookupThread);
    pinsLookupThread.start();
    toLog("GPIO OK");
}

//void DoorPhone::runTest()
//{
//    emit signalCallStart();
//    QTimer::singleShot(10000, this, &DoorPhone::stopCalling);
//}

DoorPhone::DoorPhone(QObject *parent)
    : QObject{parent}
{
    qDebug() << "Door phone started in thread: " << QThread::currentThreadId();
}
