#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTcpSocket* socket = nullptr;
    QByteArray data;
    bool connected = false;

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

    int applyVolumeToSample(short iSample);

    void startAudio();
    void playBuffer(QByteArray &buffer);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void message(QString _msg);

private slots:
    void readInput();

public slots:
    void socketReady();
    void socketDisconected();

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonDisconnect_clicked();

    void on_pushButtonSend_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
