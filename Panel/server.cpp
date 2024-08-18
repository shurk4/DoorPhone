#include "server.h"

Server::Server()
{
//    pingTimer = new QTimer();
//    connect(pingTimer, &QTimer::timeout, this, &Server::checkSockets);
}

Server::~Server()
{
    qDebug() << "Destructor!!!";

    qDebug() << "Sockets size: " << QString::number(sockets.size());

    if(!sockets.empty())
    {
        for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
        {
            socket.key()->deleteLater();
            sockets.erase(socket);
            socket--;
        }
    }
    sockets.clear();
    qDebug() << "Destructor!!!";
}

void Server::setPort(const uint _port)
{
    port = _port;
}

void Server::checkSockets()
{
    qDebug() << "Check sockets";
    if(isCalling)
    {
        qDebug() << "Is calling, return";
        return;
//        pingTimer->stop();
    }

    if(sockets.size() > 0)
    {
        qDebug() << "Socket is not empty, have " << sockets.size() << " connected clients";
        for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
        {
            if(socket.key()->state() == QAbstractSocket::UnconnectedState)
            {
                log("Socket state is disconnected");
                disconnectSocket(socket.key());
                socket.key()->deleteLater();
                sockets.erase(socket);
                socket--;
            }
        }
    }
    else
    {
        qDebug() << "Sockets empty";
        qDebug() << "Stop ping timer";
//        pingTimer->stop();
    }
}

void Server::socketDisconected()
{
    log("Client disconnected.");

    QTcpSocket *client = (QTcpSocket*)sender();

    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
    {
        if(socket.key() == client)
        {
            disconnectSocket(socket.key());
            return;
        }
    }
}

void Server::sendCommand(int _com)
{
    lanSendCommand(_com);
}

void Server::disconnectSocket(QTcpSocket *_socket)
{
    qDebug() << "Disconnect socket";
    sockets.remove(_socket);

    emit clientsListChanged();

    qDebug() << "Sockets size: " << sockets.size();
    if(sockets.empty()) emit noClientsConnected();
}

void Server::startCheckSocketsTimer()
{
    qDebug() << "Start check sockets timer";
    QTimer::singleShot(10000, this, &Server::checkSockets);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    log("Incomming connection");

    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket,SIGNAL(readyRead()),this,SLOT(socketReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(socketDisconected()));

    qDebug() << socketDescriptor << " Client connected";

    log("Incomming connection estabilished with ID: " + QString::number(socketDescriptor));
    QString toClient = "You are connect to test server with ID: " + QString::number(socketDescriptor);
    socket->write(toClient.toUtf8());
    log("Client: " + QString::number(socketDescriptor) + " connected. IP: " + socket->peerAddress().toString());

    sockets.insert(socket, 0);

    log("Hello message sended to client.");
    emit clientsListChanged();

//    if(!pingTimer->isActive())
//    {
//        qDebug() << "Start ping timer";
//        pingTimer->start(pingTime);
//    }
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
    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
    {
        socket.key()->write(text.toUtf8());
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

    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
    {
        QString temp = socket.key()->peerAddress().toString();
        clientsAdresses.push_back(temp);
    }
    return clientsAdresses;
}

void Server::run()
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

void Server::socketReady()
{
    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
    {
        if(socket.key()->state() == QAbstractSocket::ConnectedState && socket.key()->isWritable())
        {
            emit signalSendText(socket.key()->readAll());
        }
    }
}

SocketData::SocketData()
{

}

SocketData::~SocketData()
{
    qDebug() << "Socket data destructor!!!";
    socket->deleteLater();
    socket = nullptr;
//    delete socket;
}
