/**
            Сделать в настройках список интерфейсов(остановился на этом, возможно не понадобится т.к. брудкаст будет по IP)
            Сделать в настройках список адресов
Добавить фукционал в главное окно
Связать кнопки управления с сервером
Добавить вызов с сервера
**/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#include "udpnet.h"
#include "settingswindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void toLog(QString _log);

    // Network
    QTcpSocket* socket = nullptr;
    UDPNet network;
    QString ipAdr;
    int portTCP;;
    int portUDP;

    QByteArray data;
    bool connected = false;

    void startTCP();
    void startUDP();
    void stopUDP();

    // TCP commands
    void applyCommand(QString _com);
    void sendCommand(QString _com);

    // Sound
    QAudioDeviceInfo inputDeviceInfo;
    QAudioDeviceInfo outputDeviceInfo;
    QAudioFormat audioFormat;
    QAudioInput *audioInput;
    QAudioOutput *audioOutput;
    QIODevice *inputDevice;
    QIODevice *outputDevice;
    QByteArray buffer;
    int volume = 100;

    void initializeAudio();
    void createAudioInput();
    void createAudioOutput();

    void startAudio();
    void stopAudio();

    int applyVolumeToSample(short iSample);

    // Settings
    void readSettings();
    void writeSettings();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void message(QString _msg);

private slots:
    void readInput();

public slots:
    void applySettings();

    void socketReady();
    void socketDisconected();

    void readUDP(QByteArray _data);

private slots:
    void on_pushButtonSend_clicked();

    void on_lineEditMessage_returnPressed();

    void on_pushButtonSettings_clicked();

    void on_pushButtonAnswer_clicked();

    void on_pushButtonMute_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
