#ifndef OSDUPDATETHREAD_H
#define OSDUPDATETHREAD_H

#include <QObject>

class OSDUpdateThread : public QObject
{
    Q_OBJECT

public:
    OSDUpdateThread();
    ~OSDUpdateThread();

signals:
    void ReDrawOSD(void);

public slots:
    void working();
};

#endif // OSDUPDATETHREAD_H
