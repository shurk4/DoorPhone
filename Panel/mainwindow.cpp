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
    startAudio();

    startNetwork();

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
}

MainWindow::~MainWindow()
{
    writeSettins();
    delete ui;
}

void MainWindow::readSettings()
{
    toLog("\nRead settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    ui->lineEditPortUdp->setText(settings.value("UDP port").toString());
    ui->lineEditPortTcp->setText(settings.value("TCP port").toString());
//    ui->verticalSlid
    settings.endGroup();
    toLog("OK");
}

void MainWindow::writeSettins()
{
    toLog("\nWrite settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    settings.setValue("UDP port", ui->lineEditPortUdp->text());
    settings.setValue("TCP port", ui->lineEditPortTcp->text());
    settings.endGroup();
    toLog("OK");
}

void MainWindow::toLog(QString _log)
{
    qDebug() << _log;
    ui->textBrowser->append(_log);
}

void MainWindow::startNetwork()
{
    toLog("\nStarting network");
    toLog(" TCP Server");
    server.startServer(ui->lineEditPortTcp->text().toInt());
    connect(&server, &Server::signalSendText, this, &MainWindow::reciveMessage);
    toLog(" OK");

    toLog(" UDP");
    network.setPort(ui->lineEditPortUdp->text().toInt());
    network.initUdp();
    connect(&network, &UDPNet::signalData, this, &MainWindow::slotData);
    toLog(" OK");
    toLog("OK");
}

void MainWindow::listInterfaces()
{
    toLog("\nListining interfaces");
    for(auto &i : network.getInterfaces())
    {
        ui->comboBoxInterfaces->addItem(i.humanReadableName());
    }
    toLog("OK");
}

void MainWindow::listLocalAdresses()
{
    toLog("Listeniong local adresses");
    for(auto i : network.getLocalAdresses())
    {
        ui->listWidgetIPs->addItem(i.toString());
    }
    toLog("OK");
}

void MainWindow::prepareAudio()
{
    toLog("Preparing audio system");
    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    outputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audioInput = 0;
    audioOutput = 0;
    inputDevice = 0;
    outputDevice = 0;
}

void MainWindow::initAudio()
{
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
}

void MainWindow::createAudioInput()
{
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }

    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);
}

void MainWindow::createAudioOutput()
{
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
}

void MainWindow::startAudio()
{
    toLog("Audio turn on");
    //Audio output device
    outputDevice = audioOutput->start();
        //Audio input device
    inputDevice = audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(inputDevice, &QIODevice::readyRead, this, &MainWindow::readInput);
}

void MainWindow::stopAudio()
{
    toLog("Audio turn off");
//    delete outputDevice;
    outputDevice->close();
//    delete inputDevice;
    inputDevice->close();
    disconnect(inputDevice, &QIODevice::readyRead, this, &MainWindow::readInput);
}


void MainWindow::initGPIO()
{
    toLog("\nInitializin GPIO");
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
    toLog("Message: " + message);
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
