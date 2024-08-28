#include "tcpHandler.h"

TCPHandler::TCPHandler(QObject *parent)
    : QObject{parent}
{
    toLog("TCP Handler started");
    readSettings();
}

void TCPHandler::setPort(const int _port)
{
    port = _port;
}

void TCPHandler::toLog(const QString _log)
{
    QString log = "TCP Handler: " + _log;
    emit signalLog(log);
}

void TCPHandler::run()
{
    qDebug() << "TCP Class started in thread: " << QThread::currentThreadId();
    initServer();
}

void TCPHandler::slotClientList()
{
    for(auto client :server.getClientsList())
    {
        toLog(client);
    }
}

void TCPHandler::slotFromTCP(const QString _data)
{
    toLog("Recived data: " + _data);
}

void TCPHandler::slotToTCP(const QString _data)
{
    emit signalToTcp(_data);
}

void TCPHandler::readSettings()
{
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    port = settings.value("TCP port").toInt();
    toLog("TCP port:" + QString::number(port));
    settings.endGroup();
    toLog("REad settings - OK");
}

void TCPHandler::initServer()
{
    toLog("Starting TCP server");

    connect(&server, &Server::signalData, this, &TCPHandler::slotFromTCP, Qt::DirectConnection);
    connect(&server, &Server::signalLog, this, &TCPHandler::toLog);
    connect(&server, &Server::clientsListChanged, this, &TCPHandler::slotClientList);

    connect(this, &TCPHandler::signalToTcp, &server, &Server::sendData);

    connect(this, &TCPHandler::signalSetPort, &server, &Server::setPort);
    connect(this, &TCPHandler::signalStartServer, &server, &Server::run);

    emit signalSetPort(port);
    emit signalStartServer();
}

void TCPHandler::sendCommand(const int _com)
{
    QString command = "&" + QString::number(_com);
    emit signalToTcp(command);
}

void TCPHandler::slotCallStart()
{
    sendCommand(START_CALL);
}

void TCPHandler::slotCallStop()
{
    sendCommand(STOP_CALL);
}
