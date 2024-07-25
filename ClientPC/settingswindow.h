#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QNetworkInterface>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

signals:
    void settingsChanged();

private slots:
    void on_verticalSliderSpk_valueChanged(int value);

    void on_verticalSliderMic_valueChanged(int value);

    void on_pushButtonSave_clicked();

private:
    Ui::SettingsWindow *ui;

    void readSettings();
    void writeSettings();

    void showInterfaces();
    void showIPs();
};

#endif // SETTINGSWINDOW_H
