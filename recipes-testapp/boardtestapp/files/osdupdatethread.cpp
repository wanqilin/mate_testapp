#include "osdupdatethread.h"
#include <QtDebug>
#include <QThread>

OSDUpdateThread::OSDUpdateThread() {}

OSDUpdateThread::~OSDUpdateThread() {}

void OSDUpdateThread::working(void)
{
    qDebug()<<"OSDUpdateThread is run!";
    while(1)
    {
        emit ReDrawOSD();
        QThread::sleep(3);
    }
}
