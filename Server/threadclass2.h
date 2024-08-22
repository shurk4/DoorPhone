#ifndef THREADCLASS2_H
#define THREADCLASS2_H

#include <QObject>
#include <QDebug>
#include <QThread>

class ThreadClass2 : public QObject
{
    Q_OBJECT
public:
    explicit ThreadClass2(QObject *parent = nullptr);

signals:
    void sendData(QString);

public slots:
    void reciveData(QString _data);

    void run();
};

#endif // THREADCLASS2_H
