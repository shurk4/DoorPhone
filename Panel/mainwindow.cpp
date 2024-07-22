#include "mainwindow.h"
#include "./ui_mainwindow.h"
//const int BufferSize = 14096;

void MainWindow::listInterfaces()
{
    for(auto &i : network.getInterfaces())
    {
        ui->comboBoxInterfaces->addItem(i.humanReadableName());
    }
}

void MainWindow::listLocalAdresses()
{
    for(auto i : network.getLocalAdresses())
    {
        ui->listWidgetIPs->addItem(i.toString());
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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

    server.startServer(2024);
    connect(&server, &Server::signalSendText, this, &MainWindow::reciveMessage);
    connect(&server, &Server::signalSendBytes, this, &MainWindow::reciveAudio);

    network.setPort(port);
    network.initUdp();

    connect(&network, &UDPNet::signalData, this, &MainWindow::slotData);

    // Pins control
    qDebug() << "Starting wiringOP";
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
    // Pins control

    // Offed functions
    qDebug() << "\n-------------------------ATTANSION---------------------------\n";
    qDebug() << "startAudio is OFF";

    qDebug() << "\n";

    // Load UI
    listInterfaces();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readInput()
{
    //Return if audio input is null
    if(!audioInput){
        return;
    }

//    if(mute)
//    {
//        return;
//    }

//    if(echo)
//    {
    //    outputDevice->write(inputDevice->readAll());

//    }

    network.sendData(inputDevice->readAll());
}

int MainWindow::applyVolumeToSample(short iSample)
{
    //Calculate volume, Volume limited to  max 35535 and min -35535
    return std::max(std::min(((iSample * volume) / 50) ,35535), -35535);
}

void MainWindow::reciveMessage(QString message)
{
    ui->textBrowser->append(message);
}

void MainWindow::reciveAudio(QByteArray sample)
{
    ui->textBrowser->append(QString(sample));
}

void MainWindow::slotData(QByteArray _data)
{
    outputDevice->write(_data);
}

void MainWindow::initializeAudio()
{
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
}

void MainWindow::createAudioOutput()
{
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
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

void MainWindow::createAudioInput()
{
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }

    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);
}

void MainWindow::on_pushButtonSend_clicked()
{
    server.lanSendText(ui->lineEditMessage->text());
    ui->lineEditMessage->clear();
}

void MainWindow::btnStateChanged(int _pin, int _state)
{
    ui->pushButtonCall->setChecked(_state);
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
    if(digitalRead(out2Pin) == HIGH)
    {
        digitalWrite(out2Pin, LOW);
        return;
    }
    digitalWrite(out2Pin, HIGH);
}
