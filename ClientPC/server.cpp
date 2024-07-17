#include "server.h"

Server::Server()
{
    sockets.clear();
}

Server::~Server()
{
    qDebug() << "Destructor!!!";

    qDebug() << "Sockets size: " << QString::number(sockets.size());

    if(!sockets.empty())
    {
        for(size_t i = sockets.size(); i >=0 ; i--)
        {
            qDebug() << "I: " << QString::number(i) << " ID: " << QString::number(sockets[i]->socketDescriptor());
            sockets[i]->disconnect();
            sockets[i]->deleteLater();
            sockets[i] = nullptr;
        }
    }

//    while(sockets.size() != 0)
//    {
//        sockets.back()->disconnected();
//        sockets.pop_back();
//    }
    sockets.clear();
    qDebug() << "Destructor!!!";
}

void Server::closeEvent(QCloseEvent *event)
{
    event->ignore();

    event->accept();
}

void Server::message(QString msg)
{
    qDebug() << msg;
    emit sendMessage(msg);
}

void Server::startServer(uint port)
{
    if (this->listen(QHostAddress::Any, port))
    {
        message("Server started on port: " + QString::number(port));
    }
    else
    {
        qDebug()<<"Not listening";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    message("Incomming connection");

    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket,SIGNAL(readyRead()),this,SLOT(socketReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(socketDisconected()));

    qDebug()<<socketDescriptor<<" Client connected";

    message("Incomming connection estabilished with ID: " + QString::number(socketDescriptor));
    QString toClient = "You are connect to test server with ID: " + QString::number(socketDescriptor);
    socket->write(toClient.toUtf8());
    sendText("Client: " + QString::number(socketDescriptor) + " connected");

    sockets.push_back(socket);

    message("Hello message sended to client.");
}

void Server::socketReady()
{
    for(auto &socket : sockets)
    {
        data = socket->readAll();
        emit sendAudioMessage(data);

        QString tempData = data;
        QStringList strlist = tempData.split(QRegExp(":"));
        tempData = "";

        for(const auto &i : strlist)
        {
            tempData += i + " @ ";
        }

        message("Client " + QString::number(socket->socketDescriptor()) + ": " + tempData);
    }
}

void Server::socketDisconected()
{
    message("Client disconnected.");
    QTcpSocket *client = (QTcpSocket*)sender();
    for(size_t i = 0; i < sockets.size(); i++)
    {
        if(sockets[i] == client)
        {
            sendText(QString::number(sockets[i]->socketDescriptor()) + ": is disconected");
            sockets[i]->deleteLater();
            sockets.erase(sockets.begin() + i);
            return;
        }
    }
}

// Отправить клиенту текст из поля ввода
void Server::sendText(const QString text)
{
    for(const auto &socket : sockets)
    {
        socket->write(text.toUtf8());
    }
}

void Server::sendSampl(QByteArray &_sampl)
{
    for(const auto &socket : sockets)
    {
        socket->write(_sampl);
    }
}

void Server::sendAction(const QString object, const QString action)
{
    QString msg = "action:" + object + ":" + action;
    sendText(msg);
}
