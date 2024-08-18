#include "phone.h"

Phone::Phone(QObject *parent)
    : QObject{parent}
{
    emit toLog("");
    emit toLog("Preparing audio system");
    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    outputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audioInput = 0;
    audioOutput = 0;
    inputDevice = 0;
    outputDevice = 0;
    emit toLog("OK");
}

void Phone::init()
{
    emit toLog("");
    emit toLog("Initialize audio format");
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

    emit toLog("Audio is initialized");
}

void Phone::createAudioInput()
{
    emit toLog("");
    emit toLog("Create audio input");
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }
    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);
    emit toLog("OK");
}

void Phone::createAudioOutput()
{
    emit toLog("");
    emit toLog("Create audio output");
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
    emit toLog("OK");
}

void Phone::readInput()
{
    //Return if audio input is null
    if(!audioInput){
        return;
    }

    emit haveSound(inputDevice->readAll());
}

void Phone::writeOutput(QByteArray _sound)
{
    outputDevice->write(_sound);
}

void Phone::start()
{
    emit toLog("");
    emit toLog("Audio turn on");
    //Audio output device
    audioOutput->setVolume(100);
    outputDevice = audioOutput->start();
        //Audio input device
    audioInput->setVolume(100);
    inputDevice = audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(inputDevice, &QIODevice::readyRead, this, &Phone::readInput);
    emit toLog("Current speaker volume: " + QString::number(audioOutput->volume()));
    emit toLog("Current microphone voluve: " + QString::number(audioInput->volume()));

    emit toLog("OK");
}

void Phone::stop()
{
    emit toLog("");
    emit toLog("Audio turn off");
    if (inputDevice != 0)
    {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
        emit toLog(" microphone is off");
    }
    if(outputDevice != 0)
    {
        outputDevice = 0;
        emit toLog(" speakers is off");
    }
    emit toLog("OK");
}
