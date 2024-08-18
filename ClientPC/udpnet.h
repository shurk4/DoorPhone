#ifndef UDPNET_H
#define UDPNET_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkProxy>
#include <QNetworkInterface>

class UDPNet : public QObject
{
    Q_OBJECT

    QUdpSocket *udpSocket;
    uint port = 2024;

    bool online = false;

    void toLog(QString _log);

public:
    explicit UDPNet(QObject *parent = nullptr);

    QList<QString> getNetworkInterfaces();
    void setInterfaceByIndex(int index);
    int selectedInterface = 0;

    void setPort(uint _port);
    uint getPort();

    void initUdp();
    bool isOnline();

    void sendData(QByteArray _data);

    void printLocalIPs();

    QList<QHostAddress> getLocalAdresses();
    QList<QNetworkInterface> getInterfaces();

public slots:
    void readUdp();
    void socketDisconnected();


signals:
    void signalData(QByteArray);
};

#endif // UDPNET_H
