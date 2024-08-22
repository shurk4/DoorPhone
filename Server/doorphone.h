#ifndef DOORPHONE_H
#define DOORPHONE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "tcpclass.h"
#include "threadclass1.h"
#include "threadclass2.h"

class DoorPhone : public QObject
{
    Q_OBJECT

    ThreadClass1 class1;
    QThread class1Thread;
//    ThreadClass2 class2;
//    QThread class2Thread;

    TCPClass tcp;
    QThread tcpThread;

    void runTest();

public:
    explicit DoorPhone(QObject *parent = nullptr);
    void runThreads();

signals:
    void callStart(); // temp
    void callStop(); // temp

public slots:
    void toLog(QString _log);

    void stopCalling(); // temp

};

#endif // DOORPHONE_H
