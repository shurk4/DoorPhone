#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings();

    preparePhone();

    startTCP();

    // Pins control
    initGPIO();
    // Pins control

    // Offed functions
    qDebug() << "\n-------------------------ATTANSION---------------------------\n";
    qDebug() << "No attansions";
    qDebug() << "\n";

    // Load UI
    listInterfaces();
    listLocalAdresses();

    initCallPlayer();
}

MainWindow::~MainWindow()
{
    writeSettins();
    delete ui;
}

void MainWindow::readSettings()
{
    toLog("");
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    ui->lineEditCallTimer->setText(settings.value("call timer").toString());

    ui->lineEditPortUdp->setText(settings.value("UDP port").toString());
    ui->lineEditPortTcp->setText(settings.value("TCP port").toString());

    this->restoreGeometry(settings.value("window geometry").toByteArray());
    settings.endGroup();
    toLog("OK");
}

void MainWindow::writeSettins()
{
    toLog("");
    toLog("Write settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    settings.setValue("call timer", ui->lineEditCallTimer->text());
    settings.setValue("UDP port", ui->lineEditPortUdp->text());
    settings.setValue("TCP port", ui->lineEditPortTcp->text());
    settings.setValue("window geometry", this->saveGeometry());
    settings.endGroup();
    toLog("OK");
}

void MainWindow::toLog(QString _log)
{
    qDebug() << _log;
    ui->textBrowser->append(_log);
}

void MainWindow::startTCP()
{
    tcpServer = new Server();
    tcpServerThread = new QThread();
    toLog("");
    toLog("Starting TCP network");
    toLog(" TCP Server");

    tcpServer->setPort(ui->lineEditPortTcp->text().toInt());
    connect(tcpServerThread, &QThread::started, tcpServer, &Server::run);
    connect(this, &MainWindow::tcpSend, tcpServer, &Server::sendCommand);
    connect(tcpServer, &Server::signalSendText, this, &MainWindow::reciveData, Qt::DirectConnection);
    connect(tcpServer, &Server::clientsListChanged, this, &MainWindow::clientsListChanged);
    connect(tcpServer, &Server::noClientsConnected, this, &MainWindow::noClientsConnected);
    tcpServer->moveToThread(tcpServerThread);
    tcpServerThread->start(QThread::TimeCriticalPriority);
    toLog(" OK");
}

void MainWindow::listInterfaces()
{
    toLog("");
    toLog("Listining interfaces");
    for(auto &i : phone->getInterfaces())
    {
        ui->comboBoxInterfaces->addItem(i.humanReadableName());
    }
    toLog("OK");
}

void MainWindow::listLocalAdresses()
{
    toLog("");
    toLog("Listeniong local adresses");
    for(auto i : phone->getLocalAdresses())
    {
        ui->listWidgetIPs->addItem(i.toString());
    }
    toLog("OK");
}

void MainWindow::preparePhone()
{
    phone = new UDPPhone(ui->lineEditPortUdp->text().toInt());
    phoneThread = new QThread();
    connect(phoneThread, &QThread::started, phone, &UDPPhone::initAudio);
    connect(phone, &UDPPhone::signalLog, this, &MainWindow::toLog);
    connect(phone, &UDPPhone::stopped, this, &MainWindow::udpPhoneStopped);
    phone->moveToThread(phoneThread);
    phoneThread->start(QThread::TimeCriticalPriority);
}

void MainWindow::initGPIO()
{
    toLog("");
    toLog("Initializin GPIO");
    wiringPiSetup();

    pinMode(out1Pin, OUTPUT);
    pinMode(out2Pin, OUTPUT);

    buttonsPins.push_back(buttonCallPin);
    buttons.setPins(buttonsPins);

    connect(this, &MainWindow::stopPins, &buttons, &Pins::stopLoop);
    connect(&buttons, &Pins::btnStateChanged, this, &MainWindow::btnStateChanged);
    connect(&lookupSensorsThread, &QThread::started, &buttons, &Pins::run);
    connect(&buttons, &Pins::finished, &lookupSensorsThread, &QThread::terminate);

    buttons.moveToThread(&lookupSensorsThread);
    lookupSensorsThread.start();
    toLog("OK");
}

void MainWindow::applyCommand(int _com)
{
    toLog("Apply command: " + QString::number(_com));
    if(_com & ANSWER)
    {
        if(isIncommingCall & !isAnswered)
        {
            isAnswered = true;
            isIncommingCall = false;
            answer();
            sendCommand(ANSWER);
        }
    }
    if(_com & END_CALL)
    {
        if(isAnswered & !isIncommingCall)
        {
            isAnswered = false;
            stopCall();
        }
    }
    if(_com & DOOR_1)
    {
        door1();
    }
    if(_com & DOOR_2)
    {
        door2();
    }
}

void MainWindow::sendCommand(int _com)
{
    toLog("Send command: " + QString::number(_com));
    emit tcpSend(_com);
}

void MainWindow::incommingCallStart()
{
    toLog("Incomming call start");
    if(isIncommingCall || isAnswered) return;
    isIncommingCall = true;

    callMusicStart();
    sendCommand(INCOMMING_CALL);
    QTimer::singleShot(ui->lineEditCallTimer->text().toInt(), this, &MainWindow::incommingCallTimerShot);
    ui->pushButtonCall->setChecked(true);
}

void MainWindow::incommingCallStop()
{
    toLog("Incomming call stop");
    if(isIncommingCall)
    {
        callMusicStop();
        tcpServer->isCalling = false;

        sendCommand(END_CALL);
        isIncommingCall = false;
        ui->pushButtonCall->setChecked(false);
    }
    else
    {
        ui->pushButtonCall->setChecked(true);
    }
}

void MainWindow::answer()
{    
    toLog("Answer call");
    callMusicStop();

    isAnswered = true;
    ui->pushButtonTalk->setChecked(true);
    toLog("Timer deleted");
    phone->start();
    toLog("Calling answered");
}

void MainWindow::stopCall()
{
    tcpServer->isCalling = false;
    toLog("Calling end");
    ui->pushButtonTalk->setChecked(false);

    phone->stop();

    ui->pushButtonCall->setChecked(false);
    isIncommingCall = false;
}

void MainWindow::door1()
{
    toLog("Open door 1");
    if(digitalRead(out1Pin) == HIGH)
    {
        digitalWrite(out1Pin, LOW);
        return;
    }
    digitalWrite(out1Pin, HIGH);
    QTimer::singleShot(3000, this, &MainWindow::door1isClosed);
}

void MainWindow::door2()
{
    toLog("Open door 2");
    if(digitalRead(out2Pin) == HIGH)
    {
        digitalWrite(out2Pin, LOW);
        return;
    }
    digitalWrite(out2Pin, HIGH);
    QTimer::singleShot(3000, this, &MainWindow::door2isClosed);
}

void MainWindow::door1isClosed()
{
    toLog("Door 1 is closed");
    sendCommand(DOOR_1_IS_CLOSED);
    door1();
}

void MainWindow::door2isClosed()
{
    toLog("Door 2 is closed");
    sendCommand(DOOR_2_IS_CLOSED);
    door2();
}

void MainWindow::initCallPlayer()
{
    toLog("Init call player");
    connect(this, &MainWindow::callMusicStopSignal, &callPlayer, &CallPlayer::stop, Qt::DirectConnection);
    connect(this, &MainWindow::callMusicStartSignal, &callPlayer, &CallPlayer::start, Qt::DirectConnection);
    connect(&callPlayerThread, &QThread::started, &callPlayer, &CallPlayer::run);

    callPlayer.moveToThread(&callPlayerThread);
    callPlayerThread.start();

    showCallsPlaylist();
}

void MainWindow::showCallsPlaylist()
{
    toLog("Show call play list");
    ui->listWidgetSounds->clear();
    for(int i = 0; i < callPlayer.getPlaylistSize(); i++)
    {
        QString itemText = "Sound: " + QString::number(i + 1);
        ui->listWidgetSounds->addItem(itemText);
    }
}

void MainWindow::callMusicStart()
{
    toLog("CAll music start");
    callPlayer.setTrackIndex(ui->listWidgetSounds->currentRow());
    emit callMusicStartSignal(true);
}

void MainWindow::callMusicStop()
{
    toLog("Call music stop");
    emit callMusicStopSignal();
}

void MainWindow::reciveMessage(QString _message)
{
    toLog("Message: " + _message);
}

void MainWindow::reciveData(QString _data)
{
    toLog("Recived data: " + _data);
    if(_data[0] == '&')
    {
        toLog("Is command");
        applyCommand(_data.midRef(1, _data.size() - 1).toInt());
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    tcpServer->lanSendText(ui->lineEditMessage->text());
    ui->lineEditMessage->clear();
}

void MainWindow::btnStateChanged(int _pin, int _state)
{
    ui->pushButtonCall->setChecked(_state);
    incommingCallStart();
}

void MainWindow::incommingCallTimerShot()
{
    incommingCallStop();
}

void MainWindow::clientsListChanged()
{
    toLog("Clients list changed");
    QStringList clients = tcpServer->getClientList();
    if(clients.isEmpty())
    {
        toLog("Clients list empty");
    }
    else
    {
        toLog("Show clients IP's");
        ui->listWidgetClients->clear();
        for(auto i : clients)
        {
            ui->listWidgetClients->addItem(i);
        }
    }
}

void MainWindow::noClientsConnected()
{
    toLog("No connected clients");
    if(ui->pushButtonTalk->isChecked())
    {
        stopCall();
    }
    toLog("Wiating new clients");
}

void MainWindow::udpPhoneStopped()
{

}

void MainWindow::on_pushButton1_clicked()
{
    if(digitalRead(out1Pin) == HIGH)
    {
        digitalWrite(out1Pin, LOW);
        return;
    }
    digitalWrite(out1Pin, HIGH);
}

void MainWindow::on_pushButton2_clicked()
{
    door2();
}

void MainWindow::on_pushButtonCall_clicked()
{
    incommingCallStart();
}

void MainWindow::on_listWidgetSounds_itemSelectionChanged()
{
    callPlayer.setTrackIndex(ui->listWidgetSounds->currentRow());
    emit callMusicStartSignal(false);
}

