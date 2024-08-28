#include "phoneHandler.h"

void PhoneHandler::toLog(QString _log)
{
    QString log = "Phone Handler: " + _log;
    emit signalLog(log);
}

void PhoneHandler::readSettings()
{
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    callTimerTime = settings.value("Call timer time").toInt();
    toLog("Call timer time: " + QString::number(callTimerTime));
    settings.endGroup();
    toLog("REad settings - OK");
}

PhoneHandler::PhoneHandler(QObject *parent)
    : QObject{parent}
{
}

void PhoneHandler::start()
{
    toLog("Starting phone handler");
    qDebug() << "Phone handler started in thread: " << QThread::currentThreadId();
//    connect(&doorbellThread, &QThread::started, &doorbell, &CallPlayer::run);
    connect(this, &PhoneHandler::signalCallStart, &doorbell, &CallPlayer::start, Qt::DirectConnection);
    connect(this, &PhoneHandler::signalCallStop, &doorbell, &CallPlayer::stop, Qt::DirectConnection);
//    doorbell.moveToThread(&doorbellThread);
//    doorbellThread.start();

    doorbell.run();
    phone.initAudio();

    connect(&callTimer, &QTimer::timeout, this, &PhoneHandler::slotStopCall, Qt::DirectConnection);
    connect(this, SIGNAL(signalStartCallTimer(int)), &callTimer, SLOT(start(int)));
    connect(this, &PhoneHandler::signalStopCallTimer, &callTimer, &QTimer::stop, Qt::DirectConnection);
}

void PhoneHandler::slotStartCall()
{
    toLog("Slot start call");
//    doorbell.start(true);
    emit signalCallStart(true);
//    callTimer.start(callTimerTime);
    emit signalStartCallTimer(callTimerTime);
}

void PhoneHandler::slotStopCall()
{

//    callTimer.stop();
    emit signalStopCallTimer();
//    doorbell.stop();
    emit signalCallStop();
}

void PhoneHandler::slotStartPhone()
{
    slotStopCall();
    phone.start();
}

void PhoneHandler::slotStopPhone()
{
    phone.stop();
}
