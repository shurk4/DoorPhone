#include "server.h"

void Server::checkSockets()
{
    qDebug() << "Check sockets";
    if(!sockets.empty())
    {
        for(auto i = 0; i < sockets.size(); i++)
        {
            if(!sockets[i]->isValid())
            {
                qDebug() << "Socket: " << sockets[i]->socketDescriptor() << " not valid.";
                sockets[i]->deleteLater();
                sockets.erase(sockets.begin() + i);
                i--;
                qDebug() << "Deleted";
            }
        }
    }

    startCheckSocketsTimer();
}

void Server::startCheckSocketsTimer()
{
    QTimer::singleShot(3000, this, &Server::checkSockets);
}

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

void Server::log(QString msg)
{
    qDebug() << msg;
    emit signalSendText("TCP: " + msg);
}


// Отправить клиенту текст из поля ввода
void Server::lanSendText(const QString text)
{
    for(const auto &socket : sockets)
    {
        socket->write(text.toUtf8());
    }
}

void Server::lanSendCommand(int _com)
{
    QString msg = "&" + QString::number(_com);
    lanSendText(msg);
}

QStringList Server::getClientList()
{
    qDebug() << "Get client list";
    QStringList clientsAdresses;
    for(int i = 0; i < sockets.size(); i++)
    {
        QString temp = sockets[i]->peerAddress().toString();
        clientsAdresses.push_back(temp);

    }
    return clientsAdresses;
}

void Server::startServer(uint port)
{
    if (this->listen(QHostAddress::Any, port))
    {
        log("Server started on port: " + QString::number(port));
    }
    else
    {
        qDebug()<<"Not listening";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    log("Incomming connection");

    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket,SIGNAL(readyRead()),this,SLOT(socketReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(socketDisconected()));

    qDebug()<<socketDescriptor<<" Client connected";

    log("Incomming connection estabilished with ID: " + QString::number(socketDescriptor));
    QString toClient = "You are connect to test server with ID: " + QString::number(socketDescriptor);
    socket->write(toClient.toUtf8());
    log("Client: " + QString::number(socketDescriptor) + " connected. IP: " + socket->peerAddress().toString());

    sockets.push_back(socket);

    log("Hello message sended to client.");
    emit clientsListChanged();
    checkSockets();
}

void Server::socketReady()
{
    for(auto &socket : sockets)
    {
        emit signalSendText(socket->readAll());
    }
}

void Server::socketDisconected()
{
    log("Client disconnected.");
    QTcpSocket *client = (QTcpSocket*)sender();
    for(size_t i = 0; i < sockets.size(); i++)
    {
        if(sockets[i] == client)
        {
            log(QString::number(sockets[i]->socketDescriptor()) + ": is disconected");
            sockets[i]->deleteLater();
            sockets.erase(sockets.begin() + i);
            break;
        }
    }
    emit clientsListChanged();
}
