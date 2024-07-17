#include "mainwindow.h"
#include "./ui_mainwindow.h"
const int BufferSize = 14096;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , buffer(BufferSize, 0)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    outputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audioInput = 0;
    audioOutput = 0;
    inputDevice = 0;
    outputDevice = 0;

    initializeAudio();
    startAudio();

    network.setPort(port);
    network.initUdp();

    connect(&network, &UDPNet::signalData, this, &MainWindow::slotData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::message(QString msg)
{
    delete ui;
}

void MainWindow::socketReady()
{
    ui->textBrowser->append(socket->readAll());
}

void MainWindow::socketDisconected()
{
    socket->deleteLater();
    socket = nullptr;
    message("Disconnected");
    connected = false;
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if(socket == nullptr)
    {
        message("Try to connect");
        QString ip = ui->lineEditIp->text();
        int port = ui->lineEditPort->text().toInt();

        socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconected()));

        socket->connectToHost(ip, port);
        connected = true;
    }
    else
    {
        message("Alredy connected!");
    }
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    if(socket->isValid())
    {
        socket->disconnect();
    }
    socketDisconected();
}

void MainWindow::on_pushButtonSend_clicked()
{
    socket->write(ui->lineEditMessage->text().toUtf8());
    ui->lineEditMessage->clear();
}

void MainWindow::readInput()
{
    //Return if audio input is null
    if(!audioInput)
        return;

    network.sendData(inputDevice->readAll());
}

void MainWindow::initializeAudio()
{
    audioFormat.setSampleRate(8000); //set frequency to 8000
    audioFormat.setChannelCount(1); //set channels to mono
    audioFormat.setSampleSize(16); //set sample size to 16 bit
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
}

void MainWindow::createAudioOutput()
{
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
}

void MainWindow::createAudioInput()
{
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }

    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);
}

int MainWindow::applyVolumeToSample(short iSample)
{
    //Calculate volume, Volume limited to  max 35535 and min -35535
    return std::max(std::min(((iSample * volume) / 50) ,35535), -35535);
}

void MainWindow::startAudio()
{
    //Audio output device
    outputDevice = audioOutput->start();
        //Audio input device
    inputDevice = audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(inputDevice, SIGNAL(readyRead()), SLOT(readInput()));
}

void MainWindow::slotData(QByteArray _data)
{
    outputDevice->write(_data);
}
