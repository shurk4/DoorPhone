#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#include "server.h"
#include "udpnet.h"
#include "pins.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    int buttonCallPin = 11;

    int out1Pin = 12;
    int out2Pin = 14;

    void listInterfaces();
    void listLocalAdresses();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void stopPins();

public slots:
    void btnStateChanged(int _pin, int _state);

private slots:
    void readInput();
    void reciveMessage(QString message);
    void reciveAudio(QByteArray sample);

    void slotData(QByteArray _data);

    void on_pushButtonSend_clicked();

    void on_pushButton1_clicked();

    void on_pushButton2_clicked();

private:
    Ui::MainWindow *ui;
    Server server;

    UDPNet network;
    int port = 2024;

    QAudioDeviceInfo inputDeviceInfo;
    QAudioDeviceInfo outputDeviceInfo;
    QAudioFormat audioFormat;
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
    QIODevice *inputDevice;
    QIODevice *outputDevice;
    QByteArray buffer;

    int volume = 99;

    void initializeAudio();
    void createAudioInput();
    void createAudioOutput();

    int applyVolumeToSample(short iSample);

    void startAudio();
    void playBuffer(QByteArray &buffer);

    //For orangePI buttons list
    Pins buttons;
    QVector<int>buttonsPins;

    QThread lookupSensorsThread;
};
#endif // MAINWINDOW_H
