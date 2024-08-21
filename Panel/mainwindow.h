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
#include <QtConcurrent/QtConcurrent>

#include "server.h"
#include "pins.h"
#include "callplayer.h"
#include "udpphone.h"

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

    void checkTCPSockData();

public slots:
    void btnStateChanged(int _pin, int _state);
    void incommingCallTimerShot();

    void clientsListChanged();
    void noClientsConnected();

private slots:
    void reciveMessage(QString _message);
    void reciveData(QString _data);

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
    Server tcpServer;
    QTimer checkTCPTimer;

    bool isCalling = false;

    void startTCP();

    void listInterfaces();
    void listLocalAdresses();

    // Audio
    UDPPhone phone;

    void preparePhone();

    // MP3
    QTimer callTimer;
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
