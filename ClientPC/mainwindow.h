/**
    На клиенте:
        Для приминения настроек приходится пекрезагружать приложение
        Сделать автоматическое добавление в автозагрузку
        Добавить в настройках режим запуска приложения при автозагрузке трей/окно

    На сервере:
        С опозданием получает пакет по tcp о начале разговора, вылетает
        Не выключает звук при начале вызова
        Сделать отключение усилителя звука транзистором если он не нужен

    Общее:
        Сделать отправку аудиосигнала только в UDP группу

_______________________________________________________________________________________________

    Сделано:
        На клиенте:
            Добавлена проверка на наличия подключения перед отправкой команды +
            Иногда вылетает при отелючении сервера +
            Сделать обработку вида кнопок при разных условиях +
            Сделать сворачивание в трей +
            Сделать всплывающее уведомление при вызове +
            Вылетает при отключении сервера во время разговора +

        На сервере:
            Сделать обработку отключения клиентов(Иногда вылетает при отключении клиента) +
            В настройках добавить выбор вызывного сигнала +
            13.08.2024 Вылетает при отключении клиента во время разговора +
            13.08.2024 Вылетает через какое то время после отключения клиента +
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
#include <QThread>

#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QStyle>

#include "settingswindow.h"
#include "popup.h"
#include "udpphone.h"

// enum COMMANDS{
//     INCOMMING_CALL = 1,
//     END_CALL = 2,
//     ANSWER = 4,
//     DOOR_1 = 8,
//     DOOR_2 = 16,
//     DISCONNECT = 32,
//     DOOR_1_IS_OPEN = 64,
//     DOOR_2_IS_OPEN = 128,
//     DOOR_1_IS_CLOSED = 256,
//     DOOR_2_IS_CLOSED = 512
// };

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void setStyle();

    // Network
    QTcpSocket* socket = nullptr;
    // UDPNet network;
    QString ipAdr;
    int portTCP;
    int portUDP;
    QTimer *timeout;
    int timeoutTime = 5000;

    QByteArray data;
    bool connected = false;
    bool tcpBusy = false;

    void startTCP();
    // void startUDP();
    // void stopUDP();
    void callAnswer();

    // TCP commands
    void applyCommand(int _com);
    void sendCommand(int _com);

    void endCall();
    bool isAnswered = false;

    // Sound
    UDPPhone *phone;
    QThread *phoneThread;

    void preparePhone();

    // QAudioDeviceInfo inputDeviceInfo;
    // QAudioDeviceInfo outputDeviceInfo;
    // QAudioFormat audioFormat;
    // QAudioInput *audioInput;
    // QAudioOutput *audioOutput;
    // QIODevice *inputDevice;
    // QIODevice *outputDevice;
    // QByteArray buffer;
    // int volume = 100;

    // void initializeAudio();
    // void createAudioInput();
    // void createAudioOutput();

    // void startAudio();
    // void stopAudio();

    // Settings
    void readSettings();
    void writeSettings();

    // Tray
    QSystemTrayIcon *trayIcon;
    void createTrayIcon();
    void showHideWindow();

    // PopUp
    PopUp *pop;
    void preparePopUp();
    void showHidePopUp();

protected:
    void closeEvent(QCloseEvent *_event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void message(QString _msg);

private slots:
    // void readInput();

    void iconActivated(QSystemTrayIcon::ActivationReason _reason);

public slots:
    void toLog(QString _log);
    void applySettings();

    void socketReady();
    void socketConnected();
    void socketDisconected();
    void connectionTimeout();

    void UDPPhoneStopped();

    // void readUDP(QByteArray _data);

    void toMainWindow();
    void popCallClicked();
    void popDoor1Clicked();
    void popDoor2Clicked();

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
