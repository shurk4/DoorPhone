/**
    На клиенте:
        Сделать обработку вида кнопок при разных условиях
        Сделать сворачивание в трей
        Сделать всплывающее уведомление при вызове
        Сделать автоматическое добавление в автозагрузку
        Добавить режим запуска приложения при автозагрузке трей/окно

    На сервере:
        В настройках добавить выбор вызывного сигнала

    Общее:
        Сделать отправку аудиосигнала только в UDP группу

_______________________________________________________________________________________________

    Сделано:
        На клиенте:
            Добавлена проверка на наличия подключения перед отправкой команды +
            Иногда вылетает при отелючении сервера +

        На сервере:
            Сделать обработку отключения клиентов(Иногда вылетает при отключении клиента)
**/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QTimer>
#include <QFile>

#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QStyle>

#include "udpnet.h"
#include "settingswindow.h"

enum COMMANDS{
    INCOMMING_CALL = 1,
    END_CALL = 2,
    ANSWER = 4,
    DOOR_1 = 8,
    DOOR_2 = 16,
    DISCONNECT = 32,
    DOOR_1_IS_OPEN = 64,
    DOOR_2_IS_OPEN = 128,
    DOOR_1_IS_CLOSED = 256,
    DOOR_2_IS_CLOSED = 512
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void toLog(QString _log);
    void setStyle();

    // Network
    QTcpSocket* socket = nullptr;
    UDPNet network;
    QString ipAdr;
    int portTCP;;
    int portUDP;
    QTimer *timeout;
    int timeoutTime = 5000;

    QByteArray data;
    bool connected = false;

    void startTCP();
    void startUDP();
    void stopUDP();
    void callAnswer();

    // TCP commands
    void applyCommand(int _com);
    void sendCommand(int _com);

    void endCall();

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

    // Settings
    void readSettings();
    void writeSettings();

    // Tray
    QSystemTrayIcon *trayIcon;
    void createTrayIcon();
    void showHideWindow();

protected:
    void closeEvent(QCloseEvent *_event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void message(QString _msg);

private slots:
    void readInput();

    void iconActivated(QSystemTrayIcon::ActivationReason _reason);

public slots:
    void applySettings();

    void socketReady();
    void socketConnected();
    void socketDisconected();
    void connectionTimeout();

    void readUDP(QByteArray _data);

private slots:
    void on_pushButtonSettings_clicked();

    void on_pushButtonAnswer_clicked();

    void on_pushButtonMute_clicked();

    void on_pushButtonDoor1_clicked();

    void on_pushButtonDoor2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
