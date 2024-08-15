#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QSettings>
#include <QTimer>
#include <QMediaPlayer>

#include "server.h"
#include "udpnet.h"
#include "pins.h"
#include "callplayer.h"

//enum COMMANDS{
//    INCOMMING_CALL = 1,
//    END_CALL = 2,
//    ANSWER = 4,
//    DOOR_1 = 8,
//    DOOR_2 = 16,
//    DISCONNECT = 32,
//    DOOR_1_IS_OPEN = 64,
//    DOOR_2_IS_OPEN = 128,
//    DOOR_1_IS_CLOSED = 256,
//    DOOR_2_IS_CLOSED = 512,
//    PING = 1024
//};

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

    void callMusicStartSignal(bool);
    void callMusicStopSignal();

    void tcpSend(int);
    void tcpSendText(QString);

public slots:
    void btnStateChanged(int _pin, int _state);
    void incommingCallTimerShot();

    void clientsListChanged();
    void noClientsConnected();

private slots:
    void readInput();
    void reciveMessage(QString _message);
    void reciveData(QString _data);
    void slotData(QByteArray _data); // Play sound from udp

    void on_pushButtonSend_clicked();

    void on_pushButton1_clicked();

    void on_pushButton2_clicked();

    void on_pushButtonCall_clicked();

    void on_listWidgetSounds_itemSelectionChanged();

private:
    Ui::MainWindow *ui;

    // Basic
    void readSettings();
    void writeSettins();

    void toLog(QString _log);

    // Lan
    Server *tcpServer;
    QThread *tcpServerThread;
    UDPNet network;

    void startTCP();
    void startUDP();
    void stopUDP();
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

    int volume = 99;

    void prepareAudio();
    void initAudio();
    void createAudioInput();
    void createAudioOutput();
    void startAudio();
    void stopAudio();

    // MP3
    CallPlayer callPlayer;
    QThread callPlayerThread;

    void initCallPlayer();

    void showCallsPlaylist();
    void callMusicStart();
    void callMusicStop();

    //For orangePI GPIO
    QVector<int>buttonsPins;

    Pins buttons;
    QThread lookupSensorsThread;
    void initGPIO();

    // Control
    QTimer *timer;
    bool isIncommingCall = false;
    bool isAnswered = false;
    void applyCommand(int _com);
    void sendCommand(int _com);
    void incommingCallStart();
    void incommingCallStop();
    void answer();
    void stopCall();
    void door1();
    void door2();
    void door1isOpen();
    void door2isOpen();
    void door1isClosed();
    void door2isClosed();
};
#endif // MAINWINDOW_H
