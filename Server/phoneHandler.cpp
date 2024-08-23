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
    doorbell.run();
    phone.initAudio();

    connect(&callTimer, &QTimer::timeout, this, &PhoneHandler::slotStopCall);
}

void PhoneHandler::slotStartCall()
{

    doorbell.start(true);
    callTimer.start(callTimerTime);
}

void PhoneHandler::slotStopCall()
{
    callTimer.stop();
    doorbell.stop();
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
