#ifndef TCPCLASS_H
#define TCPCLASS_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QSettings>

#include "server.h"

enum COMMANDS{
    START_CALL = 1,
    STOP_CALL = 2,
    START_PHONE = 4,
    STOP_PHONE = 8,
    DOOR_1 = 16,
    DOOR_2 = 32,
    DISCONNECT = 64,
    DOOR_1_IS_OPEN = 128,
    DOOR_2_IS_OPEN = 256,
    DOOR_1_IS_CLOSED = 512,
    DOOR_2_IS_CLOSED = 1024,
    PING = 2048
};

class TCPClass : public QObject
{
    Q_OBJECT

    Server server;
    int port = 2025;

    void readSettings();
    void initServer();
    void sendCommand(const int _com);

public:
    explicit TCPClass(QObject *parent = nullptr);

    void setPort(const int _port);

signals:
    void signalLog(QString);
    void signalSendData(QString);

    void signalStartPhone();
    void signalStopPhone();
    void signalOpenDoor(int); // and door number

public slots:
    void toLog(const QString _log);

    void run();

    void slotClientList();

    void slotData(const QString _data);
    void slotSendData(const QString _data);

    void slotCallStart();
    void slotCallStop();


};

#endif // TCPCLASS_H
