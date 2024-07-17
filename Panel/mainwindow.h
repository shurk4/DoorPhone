#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readInput();
    void reciveMessage(QString message);
    void reciveAudio(QByteArray sample);

private:
    Ui::MainWindow *ui;
    Server server;

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
};
#endif // MAINWINDOW_H
