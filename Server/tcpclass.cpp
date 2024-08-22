#include "tcpclass.h"

TCPClass::TCPClass(QObject *parent)
    : QObject{parent}
{
    toLog("TCP Class started");
    readSettings();
}

void TCPClass::setPort(const int _port)
{
    port = _port;
}

void TCPClass::toLog(const QString _log)
{
    QString log = "TCP Class: " + _log;
    emit signalLog(log);
}

void TCPClass::run()
{
    qDebug() << "TCP Class started in thread: " << QThread::currentThreadId();
    initServer();
}

void TCPClass::slotClientList()
{
    for(auto client :server.getClientsList())
    {
        toLog(client);
    }
}

void TCPClass::slotData(const QString _data)
{
    toLog("Recived data: " + _data);
}

void TCPClass::slotSendData(const QString _data)
{
    emit signalSendData(_data);
}

void TCPClass::readSettings()
{
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    port = settings.value("TCP port").toInt();
    toLog("TCP port:" + QString::number(port));
    settings.endGroup();
    toLog("REad settings - OK");
}

void TCPClass::initServer()
{
    toLog("Starting TCP server");
    server.setPort(port);

    connect(&server, &Server::signalData, this, &TCPClass::slotData, Qt::DirectConnection);
    connect(&server, &Server::signalLog, this, &TCPClass::toLog);
    connect(&server, &Server::clientsListChanged, this, &TCPClass::slotClientList);

    connect(this, &TCPClass::signalSendData, &server, &Server::sendData);

    server.run();
}

void TCPClass::sendCommand(const int _com)
{
    QString command = "&" + QString::number(_com);
    emit signalSendData(command);
}

void TCPClass::slotCallStart()
{
    sendCommand(START_CALL);
}

void TCPClass::slotCallStop()
{
    sendCommand(STOP_CALL);
}
