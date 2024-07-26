#include "udpnet.h"
#include "mainwindow.h"

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
    toLog("Set UDP port: " + QString::number(_port));
}

uint UDPNet::getPort()
{
    return port;
}

void UDPNet::sendData(QByteArray _data)
{
    if(online)
    {
        udpSocket->writeDatagram(_data, QHostAddress::Broadcast, port);
    }
    // std::cout << ">";
}

void UDPNet::initUdp()
{
    toLog("Initializing UDP");
    if(!online)
    {
        udpSocket = new QUdpSocket(this);
        udpSocket->bind(QHostAddress::AnyIPv4, port);

        connect(udpSocket, &QUdpSocket::readyRead, this, &UDPNet::readUdp);
        connect(udpSocket, &QUdpSocket::disconnected, this, &UDPNet::socketDisconnected);
        online = true;

        printLocalIPs();
        toLog("Udp connection estabilished");
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
            // std::cout << "<";
            emit signalData(datagram.data());
        }
    }
}

void UDPNet::socketDisconnected()
{
    if(udpSocket != nullptr)
    {
        delete udpSocket;
        // udpSocket->disconnect();
        // udpSocket->disconnectFromHost();
        // udpSocket->deleteLater();
        // udpSocket = nullptr;
        toLog("Socket disconnected");
        QString com = "&" + QString::number(DISCONNECT);
        emit signalData(com.toUtf8());

        online = false;
    }
}

void UDPNet::printLocalIPs()
{
    toLog("\n--- LocalIPs: ");

    for(auto i : QNetworkInterface::allAddresses())
    {
        toLog(i.toString());
    }
    toLog("\n--------------\n");
}

QList<QHostAddress> UDPNet::getLocalAdresses()
{
    return QNetworkInterface::allAddresses();
}

QList<QNetworkInterface> UDPNet::getInterfaces()
{
    return::QNetworkInterface::allInterfaces();
}
