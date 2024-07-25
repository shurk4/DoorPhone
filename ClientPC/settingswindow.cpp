#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    readSettings();

    showInterfaces();
    showIPs();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_verticalSliderSpk_valueChanged(int value)
{
    ui->labelSpkVol->setText(QString::number(value));
}

void SettingsWindow::on_verticalSliderMic_valueChanged(int value)
{
    ui->labelMicSens->setText(QString::number(value));
}

void SettingsWindow::readSettings()
{
    QSettings settings("ShurkSoft", "Door phone client");
    settings.beginGroup("settings");
    ui->lineEditIP->setText(settings.value("server IP").toString());
    ui->lineEditPortTCP->setText(settings.value("TCP port").toString());
    ui->lineEditPortUDP->setText(settings.value("UDP port").toString());
    ui->verticalSliderSpk->setValue(settings.value("Spk vol").toInt());
    ui->verticalSliderMic->setValue(settings.value("Mic vol").toInt());
    settings.endGroup();
}

void SettingsWindow::writeSettings()
{
    QSettings settings("ShurkSoft", "Door phone client");
    settings.beginGroup("settings");
    settings.setValue("server IP", ui->lineEditIP->text());
    settings.setValue("TCP port", ui->lineEditPortTCP->text());
    settings.setValue("UDP port", ui->lineEditPortUDP->text());
    settings.setValue("Spk vol", ui->verticalSliderSpk->value());
    settings.setValue("Mic vol", ui->verticalSliderMic->value());
    settings.endGroup();
}

void SettingsWindow::showInterfaces()
{
    for(auto &i : QNetworkInterface::allInterfaces())
    {
        ui->listWidgetInterfaces->addItem(i.humanReadableName());
    }
}

void SettingsWindow::showIPs()
{
    for(auto &i : QNetworkInterface::allAddresses())
    {
        ui->listWidgetIPs->addItem(i.toString());
    }
}

void SettingsWindow::on_pushButtonSave_clicked()
{
    writeSettings();
}
