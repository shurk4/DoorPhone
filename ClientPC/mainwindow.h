/**
    На клиенте:
        Для приминения настроек приходится пекрезагружать приложение
        Сделать автоматическое добавление в автозагрузку
        Добавить в настройках режим запуска приложения при автозагрузке трей/окно

    На сервере:
        Добавить сохранение выбранного рингтона

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
            Сделать отключение усилителя звука транзистором если он не нужен +
            С опозданием получает пакет по tcp о начале разговора, вылетает +
            Не выключает звук при начале вызова +
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
//     START_CALL = 1,
//     STOP_CALL = 2,
//     START_PHONE = 4,
//     STOP_PHONE = 8,
//     DOOR_1 = 16,
//     DOOR_2 = 32,
//     DISCONNECT = 64,
//     DOOR_1_IS_OPEN = 128,
//     DOOR_2_IS_OPEN = 256,
//     DOOR_1_IS_CLOSED = 512,
//     DOOR_2_IS_CLOSED = 1024,
//     PING = 2048
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
    QString ipAdr; // Адрес сервера
    int portTCP;
    int portUDP;
    QTimer *timeout;
    int timeoutTime = 5000;

    QByteArray data;
    bool connected = false;
    bool tcpBusy = false;
    bool isCalling = false; // Входящий вызов
    bool isPhoneOn = false; // Разговор


    void startTCP();

    // TCP commands
    void applyCommand(int _com);
    void sendCommand(int _com);

    void endCall();

    // Sound
    UDPPhone *phone;
    QThread *phoneThread;

    void preparePhone();
    void usePhone(); // refactoring
    void startPhone();
    void stopPhone();

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
    void showPopUp();
    void hidePopUp();

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
