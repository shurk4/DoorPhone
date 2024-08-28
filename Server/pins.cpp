#include "pins.h"

void Pins::resetStates()
{
    inPinsLastStates.resize(inputPins.size());
    for(auto i : inPinsLastStates)
    {
        i = LOW;
    }
}

Pins::Pins()
{

}

Pins::Pins(const QVector<int> &_inputPins)
{
    setPins(_inputPins);
}

void Pins::setPins(QVector<int> _inputPins)
{
    inputPins = _inputPins;
    resetStates();

    qDebug() << "Set input pins:";
    for(auto &i : inputPins)
    {
        qDebug() << i;
        pinMode(i, INPUT);
    }
}

void Pins::run()
{
    qDebug() << "Pins is runned in thread: " << QThread::currentThreadId();
    while(true)
    {
        if(loopStopped)
        {
            emit finished();
            return;
        }
        for(int i = 0; i < inputPins.size(); i++)
        {
            int state = digitalRead(inputPins[i]);
            if(state != inPinsLastStates[i])
            {
                toLog("Pin state changed!");
                inPinsLastStates[i] = state;
                emit pinStateChanged(inputPins[i], state);
            }
            loopCounter++;
            if(loopCounter == 100000000)
            {
                qDebug() << "Loop thread id: " << QThread::currentThreadId();
                loopCounter = 0;
            }
        }
    }
}

void Pins::toLog(QString _log)
{
    QString log = "GPIO Pins: " + _log;
    emit signalLog(_log);
}

void Pins::stopLoop()
{
    loopStopped = true;
}
