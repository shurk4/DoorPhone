#include "mainwindow.h"
#include "./ui_mainwindow.h"
const int BufferSize = 14096;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , buffer(BufferSize, 0)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();

    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    outputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audioInput = 0;
    audioOutput = 0;
    inputDevice = 0;
    outputDevice = 0;

    initializeAudio();
    // startAudio();
    startTCP();
    // startUDP();
    ui->pushButtonAnswer->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toLog(QString _log)
{
    qDebug() << _log;
    ui->textBrowser->append(_log);
}

// Network
void MainWindow::startTCP()
{
    if(socket == nullptr)
    {
        toLog("Trying connect to TCP server");

        socket = new QTcpSocket(this);
        toLog("New socket");
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconected()));
        connect(socket, &QTcpSocket::connected, this, &MainWindow::socketConnected);
        toLog("Socket signals connected");

        socket->connectToHost(ipAdr, portTCP);
        toLog("Connecting to host");
        timeout = new QTimer;
        toLog("New timer");
        connect(timeout, &QTimer::timeout, this, &MainWindow::connectionTimeout);
        toLog("Timer signals connected");
        timeout->start(timeoutTime);
        toLog("Timer started");
    }
    else
    {
        toLog("TCP alredy connected!");
    }
}

void MainWindow::startUDP()
{
    toLog("Starting UDP network");
    network.setPort(portUDP);
    network.initUdp();

    connect(&network, &UDPNet::signalData, this, &MainWindow::readUDP);
    toLog("OK");
}

void MainWindow::stopUDP()
{
    if(network.isOnline())
    {
        toLog("Stopping UDP");
        network.socketDisconnected();
    }
}

void MainWindow::callAnswer()
{
    if(ui->pushButtonAnswer->isChecked())
    {
        startAudio();
        startUDP();
        sendCommand(ANSWER);
    }
    else
    {
        stopUDP();
        stopAudio();
        sendCommand(END_CALL);
        ui->pushButtonAnswer->setChecked(false);
        ui->pushButtonAnswer->setDisabled(true);
    }
}

void MainWindow::applyCommand(int _com)
{
    toLog("Apply command: " + QString::number(_com));
    if(_com & INCOMMING_CALL)
    {
        ui->pushButtonAnswer->setEnabled(true);
    }
    if(_com & END_CALL)
    {
        endCall();
        callAnswer();
    }
    if(_com & DISCONNECT)
    {
        endCall();
    }
}

void MainWindow::sendCommand(int _com)
{
    QString command = "&" + QString::number(_com);
    socket->write(command.toUtf8());
}

void MainWindow::endCall()
{
    ui->pushButtonAnswer->setChecked(false);
    ui->pushButtonAnswer->setDisabled(true);
    callAnswer();
}

void MainWindow::socketReady()
{
    QString data = socket->readAll();
    toLog("TCP data recived: " + data);
    if(data[0] == '&')
    {
        toLog("Is command");
        applyCommand(data.midRef(1, 1).toInt());
    }

}

void MainWindow::socketConnected()
{
    toLog("TCP connected");
    connected = true;
}

void MainWindow::socketDisconected()
{
    socket->disconnect();
    toLog("Socket signals disconnected");

    delete socket;
    toLog("Socket deleted");
    socket = nullptr;
    toLog("Socket = nullptr");
    connected = false;
    toLog("TCP disconnected");
    startTCP();
}

void MainWindow::connectionTimeout()
{
    toLog("Connection timeout signal!");
    timeout->disconnect();
    toLog("Timeout desconnected");
    delete timeout;
    toLog("Timeout deleted");

    if(!connected)
    {
        toLog("Not connected");
        socketDisconected();
        startTCP();
    }
}

void MainWindow::readInput()
{
    if(!audioInput)
        return;

    network.sendData(inputDevice->readAll());
}

void MainWindow::readUDP(QByteArray _data)
{
    outputDevice->write(_data);
}

// Audio

void MainWindow::initializeAudio()
{
    toLog("Initialize audio format");
    audioFormat.setSampleRate(8000); //set frequency to 8000
    audioFormat.setChannelCount(1); //set channels to mono
    audioFormat.setSampleSize(8); //set sample size to 16 bit
    audioFormat.setSampleType(QAudioFormat::UnSignedInt ); //Sample type as usigned integer sample
    audioFormat.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    audioFormat.setCodec("audio/pcm"); //set codec as simple audio/pcm

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
    toLog("OK");
}

void MainWindow::createAudioInput()
{
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
    toLog(" Create audio output");
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
    toLog(" OK");
}

void MainWindow::startAudio()
{
    toLog("Audio turn ON");
    //Audio output device
    outputDevice = audioOutput->start();
        //Audio input device
    inputDevice = audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(inputDevice, SIGNAL(readyRead()), SLOT(readInput()));
    toLog("OK");
}

void MainWindow::stopAudio()
{
    audioInput->stop();
    audioOutput->stop();
}

int MainWindow::applyVolumeToSample(short iSample)
{
    //Calculate volume, Volume limited to  max 35535 and min -35535
    return std::max(std::min(((iSample * volume) / 50) ,35535), -35535);
}

// Settings

void MainWindow::readSettings()
{
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door phone client");
    settings.beginGroup("settings");
    ipAdr = settings.value("server IP").toString();
    toLog("IP: " + ipAdr);
    portTCP = settings.value("TCP port").toInt();
    toLog("Port TCP: " + QString::number(portTCP));
    portUDP = settings.value("UDP port").toInt();
    toLog("Port UDP: " + QString::number(portUDP));
    settings.endGroup();
    toLog("OK");
}

void MainWindow::applySettings()
{
    toLog("Settings window is closed");
    readSettings();
}

void MainWindow::on_pushButtonSend_clicked()
{
    socket->write(ui->lineEditMessage->text().toUtf8());
    ui->lineEditMessage->clear();
}

void MainWindow::on_lineEditMessage_returnPressed()
{
    on_pushButtonSend_clicked();
}

void MainWindow::on_pushButtonSettings_clicked()
{
    SettingsWindow *sw = new SettingsWindow;
    sw->show();
    connect(sw, &SettingsWindow::settingsChanged, this, &MainWindow::applySettings);
}

void MainWindow::on_pushButtonAnswer_clicked()
{
    callAnswer();
}

void MainWindow::on_pushButtonMute_clicked()
{
    audioInput->setVolume(!ui->pushButtonMute->isChecked());
}

void MainWindow::on_pushButtonDoor1_clicked()
{
    sendCommand(DOOR_1);
}

void MainWindow::on_pushButtonDoor2_clicked()
{
    sendCommand(DOOR_2);
}

