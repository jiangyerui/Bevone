#ifndef CANRXTHREAD_H
#define CANRXTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "CanPort/canport.h"
#include "GlobalData/globaldata.h"
#include "MySqlite/mysqlite.h"

class CanRxThread : public QThread
{
    Q_OBJECT
public:
    explicit CanRxThread();
    virtual void run();
    void setCanConf(CanPort *can, int canFd, int net, uint pollTime);
    void dealCanData(struct can_frame frame);
    void nodeStatus(int net,uint id,char status);

    int m_canfd;
    CanPort *m_can;
    MySqlite m_db;

    int m_netMax;
    int m_idMax;
    int m_net;
    int m_pollTime;
signals:
    void sigMoudelRefresh();
    void sigChangeNodeColor();

public slots:

};

#endif // CANRXTHREAD_H
