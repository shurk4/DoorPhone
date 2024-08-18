#ifndef PHONE_H
#define PHONE_H

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

class Phone : public QObject
{
    Q_OBJECT

    QAudioDeviceInfo inputDeviceInfo;
    QAudioDeviceInfo outputDeviceInfo;
    QAudioFormat audioFormat;
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
    QIODevice *inputDevice;
    QIODevice *outputDevice;

    int volume = 99;

public:
    explicit Phone(QObject *parent = nullptr);

    void init();
    void createAudioInput();
    void createAudioOutput();

signals:
    void toLog(QString);

    void haveSound(QByteArray);

private slots:
    void readInput();
    void writeOutput(QByteArray _sound);

public slots:
    void start();
    void stop();

};

#endif // PHONE_H
