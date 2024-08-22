#include "threadclass1.h"

ThreadClass1::ThreadClass1(QObject *parent)
    : QObject{parent}
{

}

void ThreadClass1::reciveData(QString _data)
{
    qDebug() << "Thread 1 recived data: " << _data;
}

void ThreadClass1::run()
{
    qDebug() << "Class 1 started in thread: " << QThread::currentThreadId();
    int count = 0;
    while(true)
    {
        count ++;
        QString data = "T1 counter = " + QString::number(count);
        qDebug() << data;
        emit sendData(data);
        QThread::currentThread()->sleep(2);
    }
}
