#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings();

    prepareAudio();
    initAudio();

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
    server.startServer(ui->lineEditPortTcp->text().toInt());
    connect(&server, &Server::signalSendText, this, &MainWindow::reciveData);
    connect(&server, &Server::clientsListChanged, this, &MainWindow::clientsListChanged);
    connect(&server, &Server::noClientsConnected, this, &MainWindow::noClientsConnected);
    toLog(" OK");
}

void MainWindow::startUDP()
{
    toLog("Starting UDP network");
    network.setPort(ui->lineEditPortUdp->text().toInt());
    network.initUdp();
    connect(&network, &UDPNet::signalData, this, &MainWindow::slotData);
    toLog(" OK");
    toLog("OK");
}

void MainWindow::stopUDP()
{
    toLog("Stop UDP network");
    if(network.isOnline())
    {
        toLog("is online! Stopping UDP");
        network.socketDisconnected();
    }
}

void MainWindow::listInterfaces()
{
    toLog("");
    toLog("Listining interfaces");
    for(auto &i : network.getInterfaces())
    {
        ui->comboBoxInterfaces->addItem(i.humanReadableName());
    }
    toLog("OK");
}

void MainWindow::listLocalAdresses()
{
    toLog("");
    toLog("Listeniong local adresses");
    for(auto i : network.getLocalAdresses())
    {
        ui->listWidgetIPs->addItem(i.toString());
    }
    toLog("OK");
}

void MainWindow::prepareAudio()
{
    toLog("");
    toLog("Preparing audio system");
    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    outputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audioInput = 0;
    audioOutput = 0;
    inputDevice = 0;
    outputDevice = 0;
    toLog("OK");
}

void MainWindow::initAudio()
{
    toLog("");
    toLog("Initialize audio format");
    audioFormat.setSampleRate(8000); //set frequency to 8000
    audioFormat.setChannelCount(1); //set channels to mono
    audioFormat.setSampleSize(8); //set sample size to 16 bit
    audioFormat.setSampleType(QAudioFormat::UnSignedInt ); //Sample type as usigned integer sample
    audioFormat.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    audioFormat.setCodec("audio/pcm"); //set codec as simple audio/pc434

    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(audioFormat))
    {
        //Default format not supported - trying to use nearest
        audioFormat = infoIn.nearestFormat(audioFormat);
    }

    QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());

    if (!infoOut.isFormatSupported(audioFormat))
    {
        //Default format not supported - trying to use nearest
        audioFormat = infoOut.nearestFormat(audioFormat);
    }
    createAudioInput();
    createAudioOutput();

    toLog("Audio is initialized");
}

void MainWindow::createAudioInput()
{
    toLog("");
    toLog("Create audio input");
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }
    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);    
    toLog("OK");
}

void MainWindow::createAudioOutput()
{
    toLog("");
    toLog("Create audio output");
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
    toLog("OK");
}

void MainWindow::startAudio()
{
    toLog("");
    toLog("Audio turn on");
    //Audio output device
    audioOutput->setVolume(100);
    outputDevice = audioOutput->start();
        //Audio input device
    audioInput->setVolume(100);
    inputDevice = audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(inputDevice, &QIODevice::readyRead, this, &MainWindow::readInput);
    toLog("Current speaker volume: " + QString::number(audioOutput->volume()));
    toLog("Current microphone voluve: " + QString::number(audioInput->volume()));

    toLog("OK");
}

void MainWindow::stopAudio()
{
    toLog("");
    toLog("Audio turn off");
    if (inputDevice != 0)
    {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
        toLog(" microphone is off");
    }
    if(outputDevice != 0)
    {
        outputDevice = 0;
        toLog(" speakers is off");
    }
    toLog("OK");
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
    server.lanSendCommand(_com);
}

void MainWindow::incommingCallStart()
{
    toLog("Incomming call start");
    if(isIncommingCall) return;

    callMusicStart();
    isIncommingCall = true;
    sendCommand(INCOMMING_CALL);
    timer = new QTimer;
    connect(timer, &QTimer::timeout, this, &MainWindow::incommingCallTimerShot);
    int timerTime = ui->lineEditCallTimer->text().toInt();
    timer->start(timerTime);
    ui->pushButtonCall->setChecked(true);
}

void MainWindow::incommingCallStop()
{
    toLog("Incomming call stop");
    if(isIncommingCall)
    {
        callMusicStop();
        timer->disconnect();
        delete timer;
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
    callMusicStop();
    toLog("Answer call");
    ui->pushButtonTalk->setChecked(true);
    timer->disconnect();
    toLog("timer disconnected");
    delete timer;
    toLog("Timer deleted");
    startAudio();
    startUDP();
    toLog("Calling answered");
}

void MainWindow::stopCall()
{
    toLog("Calling end");
    ui->pushButtonTalk->setChecked(false);

    stopAudio();
    stopUDP();

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

void MainWindow::readInput()
{
    //Return if audio input is null
    if(!audioInput){
        return;
    }

    network.sendData(inputDevice->readAll());
}

int MainWindow::applyVolumeToSample(short iSample)
{
    //Calculate volume, Volume limited to  max 35535 and min -35535
    return std::max(std::min(((iSample * volume) / 50) ,35535), -35535);
}

void MainWindow::initCallPlayer()
{
    toLog("Init call player");
    connect(this, &MainWindow::callMusicStopSignal, &callPlayer, &CallPlayer::stop);
    connect(this, &MainWindow::callMusicStartSignal, &callPlayer, &CallPlayer::start);
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

void MainWindow::slotData(QByteArray _data)
{
    outputDevice->write(_data);
}

void MainWindow::on_pushButtonSend_clicked()
{
    server.lanSendText(ui->lineEditMessage->text());
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
    QStringList clients = server.getClientList();
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

