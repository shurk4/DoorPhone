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

class Server : public QTcpServer
{
    Q_OBJECT

    uint port = 5555;
//    QTcpSocket* socket = nullptr;
    QVector<QTcpSocket*> sockets;
    QByteArray data;

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

    // Получение данных от клиента
    void socketReady();
    // Отключение клиента
    void socketDisconected();

signals:
    void signalSendText(const QString);
    void signalSendBytes(QByteArray);

    void clientsListChanged();
};

#endif // SERVER_H
