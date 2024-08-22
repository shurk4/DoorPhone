#include <QCoreApplication>

#include "doorphone.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DoorPhone panel;
    panel.runThreads();

    return a.exec();
}
