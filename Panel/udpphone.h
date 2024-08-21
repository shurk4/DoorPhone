#ifndef UDPPHONE_H
#define UDPPHONE_H

#include <QObject>
// UDP
#include <QUdpSocket>
#include <QNetworkDatagram>
// #include <QNetworkProxy>
#include <QNetworkInterface>

// Phone
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#include <QObject>

class UDPPhone : public QObject
{
    Q_OBJECT
    void toLog(QString _log);
    bool started = false;

    // UDP
    QUdpSocket *udpSocket;
    uint port = 2024;
    int selectedInterface = 0; // Не реализовано

    bool online = false;

    // Phone
    QAudioDeviceInfo inputDeviceInfo;
    QAudioDeviceInfo outputDeviceInfo;
    QAudioFormat audioFormat;
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
    QIODevice *inputDevice;
    QIODevice *outputDevice;
    int volume = 100;

public:
    explicit UDPPhone(QObject *parent = nullptr);
    UDPPhone(uint _port, QObject *parent = nullptr);
    ~UDPPhone();

    void start();
    void stop();

    bool isStarted();

    // UDP
    void setPort(uint _port);
    void setInterfaceByIndex(int _index); // Не реализована

    void startUdp();
    void stopUdp();
    void printLocalIPs();

    bool isOnline();

    uint getPort();
    QList<QString> getNetworkInterfaces();
    QList<QHostAddress> getLocalAdresses();
    QList<QNetworkInterface> getInterfaces();

    // Phone
    void createAudioInput();
    void createAudioOutput();

    void startAudio();
    void stopAudio();

signals:
    void signalLog(QString);
    void stopped();

public slots:
    void initAudio();

private slots:
    void sendUdpSound();
    void playUdpSound();

    // UDP
    void socketDisconnected(); // Есть спорные команды

};

#endif // UDPPHONE_H
