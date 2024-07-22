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
    toLog("");
    toLog("Read settings");
    QSettings settings("ShurkSoft", "Door Phone Panel");
    settings.beginGroup("settings");
    ui->lineEditPortUdp->setText(settings.value("UDP port").toString());
    ui->lineEditPortTcp->setText(settings.value("TCP port").toString());

    ui->verticalSliderSpkVol->blockSignals(true);
    ui->verticalSliderSpkVol->setValue(settings.value("spkVol").toInt());
    ui->labelSpkVol->setText(settings.value("spkVol").toString());
    ui->verticalSliderSpkVol->blockSignals(false);
    ui->verticalSliderMicVol->blockSignals(true);
    ui->verticalSliderMicVol->setValue(settings.value("micVol").toInt());
    ui->labelMicVol->setText(settings.value("micVol").toString());
    ui->verticalSliderMicVol->blockSignals(false);

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
    settings.setValue("UDP port", ui->lineEditPortUdp->text());
    settings.setValue("TCP port", ui->lineEditPortTcp->text());
    settings.setValue("spkVol", ui->verticalSliderSpkVol->value());
    settings.setValue("micVol", ui->verticalSliderMicVol->value());
    settings.setValue("window geometry", this->saveGeometry());
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
    toLog("");
    toLog("Starting network");
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
    audioOutput->setVolume(ui->verticalSliderSpkVol->value());
    outputDevice = audioOutput->start();
        //Audio input device
    audioInput->setVolume(ui->verticalSliderMicVol->value());
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

void MainWindow::on_verticalSliderSpkVol_valueChanged(int value)
{
    ui->labelSpkVol->setText(QString::number(value * 10));

//    if(value == 0)
//    {
//        audioOutput->stop();
//        toLog("Speaker is muted");
//        return;
//    }

//    if (audioOutput->state() == QAudio::StoppedState)
//    {
//        audioOutput->start();
//    }

    int vol = ui->verticalSliderSpkVol->value();
    double volD = (double)vol / 10;

    audioOutput->setVolume(volD);
    toLog("Current speaker volume: " + QString::number(audioOutput->volume()));
}

void MainWindow::on_verticalSliderMicVol_valueChanged(int value)
{
    ui->labelMicVol->setText(QString::number(value * 10));
    if(value = 0)
    {
        audioInput->stop();
        return;
    }
    else if(audioInput->state() == QAudio::StoppedState)
    {
        audioInput->start();
    }

    int vol = ui->verticalSliderMicVol->value();
    double volD = (double)vol / 10;

    audioInput->setVolume(volD);
    toLog("Current microphone volume: " + QString::number(audioInput->volume()));
}

void MainWindow::on_lineEditVol_editingFinished()
{
    int vol = ui->lineEditVol->text().toInt();
    double volD = (double)vol / 10;

    std::cout << volD;
    toLog("volD: " + QString::number(volD));
    audioInput->setVolume(volD);
    toLog("Current microphone volume: " + QString::number(audioInput->volume()));
}
