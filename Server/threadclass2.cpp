#include "threadclass2.h"

ThreadClass2::ThreadClass2(QObject *parent)
    : QObject{parent}
{

}

void ThreadClass2::reciveData(QString _data)
{
    qDebug() << "Thread 2 recived data: " << _data;
}

void ThreadClass2::run()
{
    qDebug() << "Class 2 started in thread: " << QThread::currentThreadId();
    int count = 0;
    while(true)
    {
        count += 2;
        QString data = "T2 counter = " + QString::number(count);
        qDebug() << data;
        emit sendData(data);
        QThread::currentThread()->sleep(1);
    }
}
