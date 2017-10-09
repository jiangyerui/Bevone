#ifndef CANTXTHREAD_H
#define CANTXTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "CanPort/canport.h"
class CanTxThread : public QThread
{
    Q_OBJECT
public:
    explicit CanTxThread();
    virtual void run();
    void setCanConf(CanPort *can, int canFd, int net, uint pollTime);
    int m_net;
    int m_canfd;
    uint m_pollTime;
    CanPort *m_can;

signals:

public slots:
};

#endif // CANTXTHREAD_H
