#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QSettings>

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
    void slotData(QByteArray _data); // Play sound from udp

    void on_pushButtonSend_clicked();

    void on_pushButton1_clicked();

    void on_pushButton2_clicked();

private:
    Ui::MainWindow *ui;

    // Basic
    void readSettings();
    void writeSettins();

    void toLog(QString _log);

    // Lan
    Server server;
    UDPNet network;

    void startNetwork();
    void listInterfaces();
    void listLocalAdresses();

    // Audio
    QAudioDeviceInfo inputDeviceInfo;
    QAudioDeviceInfo outputDeviceInfo;
    QAudioFormat audioFormat;
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
    QIODevice *inputDevice;
    QIODevice *outputDevice;
//    QByteArray buffer;

    int volume = 99;

    void prepareAudio();
    void initAudio();
    void createAudioInput();
    void createAudioOutput();
    void startAudio();
    void stopAudio();
    void playBuffer(QByteArray &buffer);

    int applyVolumeToSample(short iSample);

    //For orangePI GPIO
    QVector<int>buttonsPins;

    Pins buttons;
    QThread lookupSensorsThread;
    void initGPIO();
};
#endif // MAINWINDOW_H
