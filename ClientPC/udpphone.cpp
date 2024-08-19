#include "udpphone.h"

UDPPhone::UDPPhone(QObject *parent)
    : QObject{parent}
{
    toLog("UDP Phone created with default port");
}

UDPPhone::UDPPhone(uint _port, QObject *parent) : port(_port)
{
    toLog("UDP Phone created with port: " + QString::number(port));
}

void UDPPhone::start()
{
    toLog("Starting");
    startAudio();
    startUdp();
    started = true;
    toLog("OK");
}

void UDPPhone::stop()
{
    if(started)
    {
        toLog("Stopping");
        started = false;
        stopUdp();
        // stopAudio();
        toLog("OK");
    }
}

bool UDPPhone::isStarted()
{
    return started;
}

void UDPPhone::toLog(QString _log)
{
    QString log = "UDP Phone: " + _log;
    emit signalLog(log);
}

QList<QString> UDPPhone::getNetworkInterfaces()
{
    toLog("Command: Get network interface");
    QList<QString> names;
    for(auto &i : QNetworkInterface::allInterfaces())
    {
        names.push_back(i.humanReadableName());
    }
    return names;
}

void UDPPhone::setInterfaceByIndex(int _index)
{
    selectedInterface = _index;
}

void UDPPhone::setPort(uint _port)
{
    toLog("Set port " + QString::number(_port));
    port = _port;
}

uint UDPPhone::getPort()
{
    return port;
}

void UDPPhone::startUdp()
{
    toLog("Starting UDP");
    if(!online)
    {
        udpSocket = new QUdpSocket(this);
        udpSocket->bind(QHostAddress::AnyIPv4, port);

        connect(udpSocket, &QUdpSocket::readyRead, this, &UDPPhone::playUdpSound, Qt::DirectConnection);
        connect(udpSocket, &QUdpSocket::disconnected, this, &UDPPhone::socketDisconnected);
        online = true;

        toLog("Udp connection estabilished");
        return;
    }
    toLog("Already connected");
}

void UDPPhone::stopUdp()
{
    toLog("Stopping UDP");
    socketDisconnected();
    toLog("OK");
}

bool UDPPhone::isOnline()
{
    return online;
}

void UDPPhone::printLocalIPs()
{
    toLog("\n--- LocalIPs: ");
    for(auto &i : QNetworkInterface::allAddresses())
    {
        toLog(i.toString());
    }
    toLog("\n--------------\n");
}

QList<QHostAddress> UDPPhone::getLocalAdresses()
{
    return QNetworkInterface::allAddresses();
}

QList<QNetworkInterface> UDPPhone::getInterfaces()
{
    return::QNetworkInterface::allInterfaces();
}

void UDPPhone::initAudio()
{
    toLog("Starting to initialize the audio");
    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    outputDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    audioInput = 0;
    audioOutput = 0;
    inputDevice = 0;
    outputDevice = 0;

    toLog("Prepare audio format");
    audioFormat.setSampleRate(8000); //set frequency to 8000
    audioFormat.setChannelCount(1); //set channels to mono
    audioFormat.setSampleSize(8); //set sample size to 16 bit
    audioFormat.setSampleType(QAudioFormat::UnSignedInt ); //Sample type as usigned integer sample
    audioFormat.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    audioFormat.setCodec("audio/pcm"); //set codec as simple audio/pcm

    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(audioFormat))
    {
        toLog("Default input format not supported - trying to use nearest");
        audioFormat = infoIn.nearestFormat(audioFormat);
    }

    QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());

    if (!infoOut.isFormatSupported(audioFormat))
    {
        toLog("Default output format not supported - trying to use nearest");
        audioFormat = infoOut.nearestFormat(audioFormat);
    }

    createAudioInput();
    createAudioOutput();

    toLog("Audio initialization is complete");
}

void UDPPhone::createAudioInput()
{
    toLog("Create audio input");
    if (inputDevice != 0) {
        disconnect(inputDevice, 0, this, 0);
        inputDevice = 0;
    }

    audioInput = new QAudioInput(inputDeviceInfo, audioFormat, this);
    toLog("OK");
}

void UDPPhone::createAudioOutput()
{
    toLog("Create audio output");
    audioOutput = new QAudioOutput(outputDeviceInfo, audioFormat, this);
    toLog("OK");
}

void UDPPhone::startAudio()
{
    toLog("Audio turn ON");
    //Audio output device
    outputDevice = audioOutput->start();
    //Audio input device
    inputDevice = audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(inputDevice, &QIODevice::readyRead, this, &UDPPhone::sendUdpSound);
    toLog("OK");
}

void UDPPhone::stopAudio()
{
    toLog("Stop audio");
    disconnect(inputDevice, &QIODevice::readyRead, this, &UDPPhone::sendUdpSound);
    audioInput->stop();
    audioOutput->stop();
}

void UDPPhone::sendUdpSound()
{
    if(!audioInput)
        return;

    if(online)
    {
        try {
            udpSocket->writeDatagram(inputDevice->readAll(), QHostAddress::Broadcast, port);
        } catch (...) {
            toLog("CATCH!!! Read input couldn't send data to network!");
        }
    }
}

void UDPPhone::playUdpSound()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        if(!QNetworkInterface::allAddresses().contains(datagram.senderAddress())) // Не читать свою датаграму
        {
            try {
                outputDevice->write(datagram.data());
            } catch (...) {
                toLog("Read UDP could not write to audio device!");
            }
        }
    }
}

void UDPPhone::socketDisconnected()
{
    toLog("UDP socket disconnected");
    if(udpSocket != nullptr)
    {
        disconnect(udpSocket, &QUdpSocket::readyRead, this, &UDPPhone::playUdpSound);
        disconnect(udpSocket, &QUdpSocket::disconnected, this, &UDPPhone::socketDisconnected);

        udpSocket->deleteLater();
        udpSocket = nullptr;
        stopAudio();
        emit stopped();

        online = false;
    }
}

