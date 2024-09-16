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
    toLog("");
    toLog("Starting TCP network");
    toLog(" TCP Server");
//    tcpServer.setPort(ui->lineEditPortTcp->text().toInt());
    connect(this, &MainWindow::tcpSetPort, &tcpServer, &Server::setPort);
    connect(this, &MainWindow::tcpSend, &tcpServer, &Server::sendCommand);
//    connect(&checkTCPTimer, &QTimer::timeout, &tcpServer, &Server::checkSocketsData);
    connect(&tcpServer, &Server::signalData, this, &MainWindow::reciveData, Qt::DirectConnection);
    connect(&tcpServer, &Server::signalLog, this, &MainWindow::toLog);
    connect(&tcpServer, &Server::clientsListChanged, this, &MainWindow::clientsListChanged);
    connect(&tcpServer, &Server::noClientsConnected, this, &MainWindow::noClientsConnected);

    connect(&tcpServerThread, &QThread::started, &tcpServer, &Server::run);
    emit tcpSetPort(ui->lineEditPortTcp->text().toInt());
//    tcpServer.moveToThread(&tcpServerThread);
//    tcpServerThread.start();

    tcpServer.run();
    toLog(" OK");
}

void MainWindow::listInterfaces()
{
    toLog("");
    toLog("Listining interfaces");
    for(auto &i : phone.getInterfaces())
    {
        ui->comboBoxInterfaces->addItem(i.humanReadableName());
    }
    toLog("OK");
}

void MainWindow::listLocalAdresses()
{
    toLog("");
    toLog("Listeniong local adresses");
    for(auto i : phone.getLocalAdresses())
    {
        ui->listWidgetIPs->addItem(i.toString());
    }
    toLog("OK");
}

void MainWindow::preparePhone()
{
    connect(&phone, &UDPPhone::signalLog, this, &MainWindow::toLog);
    phone.initAudio();
}

void MainWindow::initGPIO()
{
    toLog("");
    toLog("Initializin GPIO");
    wiringPiSetup();

    // doors
    pinMode(out1Pin, OUTPUT);
    pinMode(out2Pin, OUTPUT);
    digitalWrite(out1Pin, LOW);
    digitalWrite(out2Pin, LOW);
    // sound amplifier
    pinMode(amplifierPin, OUTPUT);
    digitalWrite(amplifierPin, HIGH);

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
    if(_com & START_PHONE)
    {
        if(isIncommingCall & !isAnswered)
        {
            isAnswered = true;
            isIncommingCall = false;
            answer();
            sendCommand(START_PHONE);
        }
    }
    if(_com & STOP_PHONE)
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

    digitalWrite(amplifierPin, LOW);
    isIncommingCall = true;
    callMusicStart();
    sendCommand(START_CALL);
    callTimer.start(ui->lineEditCallTimer->text().toInt());
//    checkTCPTimer.start(1000);
}

void MainWindow::incommingCallStop()
{
    toLog("Incomming call stop");
    if(isIncommingCall && !isAnswered)
    {
//        checkTCPTimer.stop();
        callTimer.stop();
        digitalWrite(amplifierPin, HIGH);
//        callMusicStop();
        emit callMusicStopSignal();
        isCalling = false;

        sendCommand(STOP_CALL);
        isIncommingCall = false;
    }
}

void MainWindow::answer()
{
//    checkTCPTimer.stop();
    callTimer.stop();
    toLog("Answer call");
//    callMusicStop();
    emit callMusicStopSignal();

    isAnswered = true;
    ui->pushButtonTalk->setChecked(true);
    phone.start();
    toLog("Calling answered");
}

void MainWindow::stopCall()
{
    isCalling = false;
    toLog("Calling end");

    phone.stop();
    digitalWrite(amplifierPin, HIGH);

    isIncommingCall = false;
}

void MainWindow::door1()
{
    if(digitalRead(out1Pin) != HIGH)
    {
        toLog("Open the door 1");
        digitalWrite(out1Pin, HIGH);
        QTimer::singleShot(3000, this, &MainWindow::door1isClosed);
    }
    else
    {
        toLog("Door 1 is already opened");
    }
}

void MainWindow::door2()
{
    if(digitalRead(out2Pin) != HIGH)
    {
        toLog("Open the door 2");
        digitalWrite(out2Pin, HIGH);
        QTimer::singleShot(3000, this, &MainWindow::door2isClosed);
    }
    else
    {
        toLog("Door 2 is already opened");
    }
}

void MainWindow::door1isClosed()
{
    toLog("Door 1 is closed");
    sendCommand(DOOR_1_IS_CLOSED);
    digitalWrite(out1Pin, LOW);
}

void MainWindow::door2isClosed()
{
    toLog("Door 2 is closed");
    sendCommand(DOOR_2_IS_CLOSED);
    digitalWrite(out2Pin, LOW);
}

void MainWindow::initCallPlayer()
{
    connect(this, &MainWindow::callMusicStopSignal, &callPlayer, &CallPlayer::stop, Qt::DirectConnection);
    connect(this, &MainWindow::callMusicStartSignal, &callPlayer, &CallPlayer::start);
    connect(&callTimer, &QTimer::timeout, this, &MainWindow::incommingCallTimerShot);

    connect(&callPlayerThread, &QThread::started, &callPlayer, &CallPlayer::run);
    connect(this, &MainWindow::setRingtoneIdx, &callPlayer, &CallPlayer::setTrackIndex);

    callPlayer.moveToThread(&callPlayerThread);
    callPlayerThread.start();
//    callPlayer.run();

    showCallsPlaylist();
}

void MainWindow::showCallsPlaylist()
{
    toLog("Show call play list");
    ui->listWidgetSounds->clear();
//    for(int i = 0; i < callPlayer.getPlaylistSize(); i++)
//    {
//        QString itemText = "Sound: " + QString::number(i + 1);
//        ui->listWidgetSounds->addItem(itemText);
//    }
}

void MainWindow::callMusicStart()
{
    toLog("CAll music start");
//    callPlayer.setTrackIndex(ui->listWidgetSounds->currentRow());

    emit setRingtoneIdx(ui->listWidgetSounds->currentRow());
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
    else
    {
        ui->textBrowser->append(_data);
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
//    tcpServer.lanSendText(ui->lineEditMessage->text());
    ui->lineEditMessage->clear();
}

void MainWindow::btnStateChanged(int _pin, int _state)
{
//    ui->pushButtonCall->setChecked(_state);
    incommingCallStart();
}

void MainWindow::incommingCallTimerShot()
{
    incommingCallStop();
}

void MainWindow::clientsListChanged()
{
    toLog("Clients list changed");
    QStringList clients = tcpServer.getClientsList();
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

void MainWindow::on_pushButton1_clicked()
{
    door1();
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
//    callPlayer.setTrackIndex(ui->listWidgetSounds->currentRow());
    emit setRingtoneIdx(ui->listWidgetSounds->currentRow());
    emit callMusicStartSignal(false);
}

