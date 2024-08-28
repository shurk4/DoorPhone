#ifndef POPUP_H
#define POPUP_H

#pragma once

#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QGuiApplication>
#include <QPropertyAnimation>
#include <QPainter>
#include <QDebug>

enum COMMANDS{
    START_CALL = 1,
    STOP_CALL = 2,
    START_PHONE = 4,
    STOP_PHONE = 8,
    DOOR_1 = 16,
    DOOR_2 = 32,
    DISCONNECT = 64,
    DOOR_1_IS_OPEN = 128,
    DOOR_2_IS_OPEN = 256,
    DOOR_1_IS_CLOSED = 512,
    DOOR_2_IS_CLOSED = 1024,
    PING = 2048
};

namespace Ui {
class PopUp;
}

class PopUp : public QWidget
{
    Q_OBJECT

    // Свойство полупрозрачности
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;

public:
    explicit PopUp(QWidget *parent = 0);
    ~PopUp();

protected:
    void paintEvent(QPaintEvent *event);    // Фон будет отрисовываться через метод перерисовки

signals:
    void toMainWindow();

    void callClicked();
    void door1Clicked();
    void door2Clicked();

public slots:
    void show();                            /* Собственный метод показа виджета
                                             * Необходимо для преварительной настройки анимации
                                             * */
    void hide();
    void hideAnimation();                   // Слот для запуска анимации скрытия

private slots:

    void on_pushButtonCall_clicked();

    void on_pushButtonDoor1_clicked();

    void on_pushButtonDoor2_clicked();

    void on_pushButtonUp_clicked();

    void on_pushButtonClose_clicked();

private:
    Ui::PopUp *ui;

    QPropertyAnimation animation;   // Свойство анимации для всплывающего сообщения
    float popupOpacity;     // Свойства полупрозрачности виджета
};

#endif // POPUP_H
