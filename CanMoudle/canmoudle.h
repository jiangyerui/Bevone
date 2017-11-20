#ifndef CANMOUDLE_H
#define CANMOUDLE_H

#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <QSocketNotifier>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <net/if.h>
#include <sys/uio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


#include "GlobalData/globaldata.h"

#define PF_CAN 29

class CanMoudle : public QObject
{
    Q_OBJECT
public:
    explicit CanMoudle(QObject *parent = 0);
    CanMoudle(const char *canName,int net,uint pollTime);
    QTimer *m_canRxTimer;
    QTimer *m_canTxTimer;

    void  initCan(const char *canName);
    int dataWrite(int canfd, can_frame frame); //向串口写入数据
    int dataRead(int canfd, can_frame &frame);  //从串口读出数据

    void dealCanData(struct can_frame frame);
    void nodeStatus(int net,uint id,char status);
    void controlTimer(bool flag);
    uint m_id;
    int m_net;
    int m_canfd;
    uint m_idNum;
//    uint m_leakTimes;
//    uint m_tempTimes;

    int m_can_1;
    int m_can_2;
    void getNodeNum(Exe_Cmd exeCmd[NETNUM][CMDEXENUM]);

private:
    struct ifreq m_ifr;
    struct sockaddr_can m_addr;
signals:
    void sigSuccess();
    void sigReceiveLed();

public slots:
    void slotCanRxTimeOut();
    void slotCanTxTimeOut();
};

#endif // CANMOUDLE_H
