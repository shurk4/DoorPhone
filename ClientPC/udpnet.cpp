#include "udpnet.h"

void UDPNet::toLog(QString _log)
{
    qDebug() << _log;
    // emit signalData(_log);
}

UDPNet::UDPNet(QObject *parent)
    : QObject{parent}
{}

QList<QString> UDPNet::getNetworkInterfaces()
{
    QList<QString> names;
    for(auto &i : QNetworkInterface::allInterfaces())
    {
        names.push_back(i.humanReadableName());
    }
    return names;
}

void UDPNet::setInterfaceByIndex(int index)
{
    selectedInterface = index;
}

void UDPNet::setPort(uint _port)
{
    port = _port;
    toLog("New port: " + QString::number(_port));
}

uint UDPNet::getPort()
{
    return port;
}

void UDPNet::sendData(QByteArray _data)
{
    udpSocket->writeDatagram(_data, QHostAddress::Broadcast, port);
    toLog(">");
}

void UDPNet::initUdp()
{
    if(!online)
    {
        udpSocket = new QUdpSocket(this);
        // udpSocket->setProxy(QNetworkProxy::NoProxy); // !!! Не помогло
        // udpSocket->setMulticastInterface(QNetworkInterface::interfaceFromIndex(selectedInterface)); // Не помогло
        udpSocket->bind(QHostAddress::AnyIPv4, port);

        connect(udpSocket, &QUdpSocket::readyRead, this, &UDPNet::readUdp);
        connect(udpSocket, &QUdpSocket::disconnected, this, &UDPNet::socketDisconnected);
        toLog("Udp connection estabilished");
        online = true;
        return;
    }
    toLog("Already connected");
}

bool UDPNet::isOnline()
{
    return online;
}

void UDPNet::readUdp()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        if(!QNetworkInterface::allAddresses().contains(datagram.senderAddress()))
        {
            toLog("<");
            emit signalData(datagram.data());
        }
    }
}

void UDPNet::socketDisconnected()
{
    udpSocket->disconnectFromHost();
    udpSocket->deleteLater();
    udpSocket = nullptr;
    toLog("Socket disconnected");

    online = false;
}
