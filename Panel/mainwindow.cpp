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

    server.startServer(2024);
    connect(&server, &Server::signalSendText, this, &MainWindow::reciveMessage);
    connect(&server, &Server::signalSendBytes, this, &MainWindow::reciveAudio);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readInput()
{
    //Return if audio input is null
    if(!audioInput)
        return;

    //Check the number of samples in input buffer
    qint64 len = audioInput->bytesReady();

    //Limit sample size
    if(len > 4096)
        len = 4096;
    //Read sound samples from input device to buffer
    qint64 l = inputDevice->read(buffer.data(), len);
    if(l > 0)
    {
        //Assign sound samples to short array
        short* resultingData = (short*)buffer.data();


        short *outdata=resultingData;
        outdata[ 0 ] = resultingData [ 0 ];

        int iIndex;
        // if(ui->chkRemoveNoise->checkState() == Qt::Checked)
        // {
        //     //Remove noise using Low Pass filter algortm[Simple algorithm used to remove noise]
        //     for ( iIndex=1; iIndex < len; iIndex++ )
        //     {
        //         outdata[ iIndex ] = 0.333 * resultingData[iIndex ] + ( 1.0 - 0.333 ) * outdata[ iIndex-1 ];
        //     }
        // }

        // for ( iIndex=0; iIndex < len; iIndex++ )
        // {
        //     //Cange volume to each integer data in a sample
        //     outdata[ iIndex ] = applyVolumeToSample( outdata[ iIndex ]);
        // }

        //write modified sond sample to outputdevice for playback audio
        //!!! Сюда добавить отправку в сеть!!!
        // outputDevice->write((char*)outdata, len);
        server.lanSendBytes(buffer);
    }
}

void MainWindow::reciveMessage(QString message)
{
    ui->textBrowser->append(message);
}

void MainWindow::reciveAudio(QByteArray sample)
{
    outputDevice->write(sample);
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

void MainWindow::createAudioInput()
{
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }

    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);
}
