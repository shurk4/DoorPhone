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

    void message(const QString msg);
    void sendText(const QString text);
    void sendAction(const QString object, const QString action);
    void sendSampl(QByteArray &_sampl);

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
    void sendMessage(const QString);
    void sendAudioMessage(QByteArray _sampl);
};

#endif // SERVER_H
