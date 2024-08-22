#include "server.h"

Server::Server()
{}

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

void Server::socketDisconected()
{
    toLog("Client disconnected.");

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

void Server::sendCommand(const int _com)
{
    lanSendCommand(_com);
}

void Server::sendData(const QString _data)
{
    toLog("Send data");
    toLog("Connected clients: " + QString::number(sockets.size()));
    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
    {
        socket.key()->write(_data.toUtf8());
        socket.key()->flush();
    }
}

void Server::checkSocketsData()
{
    toLog("-- Check sockets data");
    socketReady();
}

void Server::disconnectSocket(QTcpSocket *_socket)
{
    qDebug() << "Disconnect socket";
    sockets.remove(_socket);

    emit clientsListChanged();

    qDebug() << "Sockets size: " << sockets.size();
    if(sockets.empty()) emit noClientsConnected();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    toLog("Incomming connection");

    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconected()));

    qDebug() << socketDescriptor << " Client connected";

    toLog("Incomming connection estabilished with ID: " + QString::number(socketDescriptor));
    QString toClient = "You are connect to test server with ID: " + QString::number(socketDescriptor);
    socket->write(toClient.toUtf8());
    toLog("Client: " + QString::number(socketDescriptor) + " connected. IP: " + socket->peerAddress().toString());

    sockets.insert(socket, 0);

    toLog("Hello message sended to client.");
    emit clientsListChanged();
}

void Server::closeEvent(QCloseEvent *event)
{
    event->ignore();

    event->accept();
}

void Server::toLog(QString msg)
{
    emit signalLog("TCP: " + msg);
}

//// Отправить клиенту текст из поля ввода
//void Server::lanSendText(const QString text)
//{
//    toLog("Send text");
//    toLog("Connected clients: " + QString::number(sockets.size()));
//    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
//    {
//        socket.key()->write(text.toUtf8());
//        socket.key()->flush();
//    }
//}

void Server::lanSendCommand(int _com)
{
    QString command = "&" + QString::number(_com);
    sendData(command);
}

QStringList Server::getClientsList()
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
        toLog("Server started on port: " + QString::number(port));
    }
    else
    {
        qDebug()<<"Not listening";
    }
    qDebug() << "TCP server thread: " << QThread::currentThreadId();
}

void Server::socketReady()
{
    for(auto socket = sockets.begin(); socket != sockets.end(); socket++)
    {
        toLog("Socket bytes available: " + QString::number(socket.key()->bytesAvailable()));
        if(socket.key()->state() == QAbstractSocket::ConnectedState && socket.key()->bytesAvailable())
        {
            emit signalData(socket.key()->readAll());
        }
    }
}
