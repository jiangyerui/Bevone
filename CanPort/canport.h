#ifndef CANPORT_H
#define CANPORT_H

#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QSocketNotifier>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/uio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>


#define PF_CAN 29

class CanPort
{
public:
    CanPort();

    int  initCan(const char *canName);
    void dataWrite(int canfd, can_frame frame); //向串口写入数据
    int dataRead(int canfd, can_frame &frame);  //从串口读出数据
private:
    struct ifreq m_ifr;
    struct sockaddr_can m_addr;
};

#endif // CANPORT_H
