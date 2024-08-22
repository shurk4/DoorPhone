#ifndef THREADCLASS1_H
#define THREADCLASS1_H

#include <QObject>
#include <QDebug>
#include <QThread>

class ThreadClass1 : public QObject
{
    Q_OBJECT
public:
    explicit ThreadClass1(QObject *parent = nullptr);

signals:
    void sendData(QString);

public slots:
    void reciveData(QString _data);

    void run();
};

#endif // THREADCLASS1_H
