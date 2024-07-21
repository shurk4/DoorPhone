#ifndef PINS_H
#define PINS_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <wiringPi.h>

class Pins : public QObject
{
    Q_OBJECT

    QVector<int> inputPins;
    QVector<int> inPinsLastStates;

    bool loopStopped = false;
    int loopCounter = 0;

    void resetStates();
public:
    Pins();
    Pins(const QVector<int> &_inputPins);

    void setPins(QVector<int> _inputPins);

signals:
    void btnStateChanged(int, int);
    void finished();

public slots:
    void run();
    void stopLoop();
};

#endif // PINS_H
