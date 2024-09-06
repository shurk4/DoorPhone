#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTimer>
#include <QMap>
#include <QThread>

//enum COMMANDS{ // old commands
//    INCOMMING_CALL = 1,
//    END_CALL = 2,
//    ANSWER = 4,
//    DOOR_1 = 8,
//    DOOR_2 = 16,
//    DISCONNECT = 32,
//    DOOR_1_IS_OPEN = 64,
//    DOOR_2_IS_OPEN = 128,
//    DOOR_1_IS_CLOSED = 256,
//    DOOR_2_IS_CLOSED = 512,
//    PING = 1024
//};

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

class Server : public QTcpServer
{
    Q_OBJECT

    uint port = 5555;
    QMap<QTcpSocket*, int> sockets;
    QByteArray data;

public:
    Server();
    ~Server();

    void closeEvent(QCloseEvent *event);

    // отправить сообщение в лог и по сети
    void toLog(const QString msg);

    void lanSendText(const QString text);
    void lanSendCommand(int _com);

    QStringList getClientsList();

    bool isCalling = false;

public slots:
    // Запуск сервера
    void setPort(const uint _port);
    void run();
    // Обработка входящих соединений
    void incomingConnection(const qintptr socketDescriptor);
    void disconnectSocket(QTcpSocket *_socket);

    // Получение данных от клиента
    void socketReady();
    // Отключение клиента
    void socketDisconected();
    void sendCommand(int _com);
    // Start check sockets data timer
    void checkSocketsData();

signals:
    void signalData(const QString);
    void signalLog(const QString);

    void clientsListChanged();
    void noClientsConnected();
};

#endif // SERVER_H
