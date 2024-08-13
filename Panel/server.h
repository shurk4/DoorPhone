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

struct SocketData
{
    SocketData();
//    SocketData(SocketData &_other);
    ~SocketData();
    QTcpSocket *socket = nullptr;
    int pingTimes = 0;
};

enum COMMANDS{
    INCOMMING_CALL = 1,
    END_CALL = 2,
    ANSWER = 4,
    DOOR_1 = 8,
    DOOR_2 = 16,
    DISCONNECT = 32,
    DOOR_1_IS_OPEN = 64,
    DOOR_2_IS_OPEN = 128,
    DOOR_1_IS_CLOSED = 256,
    DOOR_2_IS_CLOSED = 512,
    PING = 1024
};

class Server : public QTcpServer
{
    Q_OBJECT

    uint port = 5555;
//    QTcpSocket* socket = nullptr;
//    QVector<QTcpSocket*> sockets;
//    QMap<int, SocketData> sockets;
    QMap<QTcpSocket*, int> sockets;
    QByteArray data;
    SocketData sData;

    void startCheckSocketsTimer();
    QTimer *timer;

public:
    Server();
    ~Server();

    void closeEvent(QCloseEvent *event);

    // отправить сообщение в лог и по сети
    void log(const QString msg);

    void lanSendText(const QString text);
    void lanSendCommand(int _com);

    QStringList getClientList();

public slots:
    void checkSockets();
    // Запуск сервера
    void startServer(const uint port);
    // Обработка входящих соединений
    void incomingConnection(const qintptr socketDescriptor);
    void disconnectSocket(QTcpSocket *_socket);

    // Получение данных от клиента
    void socketReady();
    // Отключение клиента
    void socketDisconected();

signals:
    void signalSendText(const QString);
    void signalSendBytes(QByteArray);

    void clientsListChanged();
    void noClientsConnected();
};

#endif // SERVER_H
