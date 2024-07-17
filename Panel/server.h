#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

class Server : public QTcpServer
{
    Q_OBJECT

    uint port = 5555;
//    QTcpSocket* socket = nullptr;
    QVector<QTcpSocket*> sockets;
    QByteArray data;

public:
    Server();
    ~Server();

    void closeEvent(QCloseEvent *event);

    // отправить сообщение в лог и по сети
    void log(const QString msg);

    void lanSendText(const QString text);
    void lanSendAction(const QString object, const QString action);
    void lanSendBytes(const QByteArray &_sampl);

public slots:
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
};

#endif // SERVER_H
