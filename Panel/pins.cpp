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
            if(digitalRead(inputPins[i]) != inPinsLastStates[i])
            {
                inPinsLastStates[i] = digitalRead(inputPins[i]);
                emit btnStateChanged(i, digitalRead(inputPins[i]));
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

void Pins::stopLoop()
{
    loopStopped = true;
}
