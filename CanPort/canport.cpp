#include "canport.h"
#include "CanPort/canport.h"
#include <QDebug>

//#define debug

CanPort::CanPort()
{

}

int CanPort::initCan(const char* canName)
{
    int canfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(canfd == -1)
    {
        qDebug("socket");
        return 0;
    }
    else
    {
        strcpy(m_ifr.ifr_name, canName );
        if(ioctl(canfd, SIOCGIFINDEX, &m_ifr))
        {
            qDebug()<<"ioctl error";
        }

        m_addr.can_family = PF_CAN;
        m_addr.can_ifindex = m_ifr.ifr_ifindex;
        if(bind(canfd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0)
        {
            qDebug()<<"bind error";
            return -1;
        }

        qDebug()<<"**** Open "<<canName<<" OK **** ";
    }
    return canfd;

}

//发送数据
void CanPort::dataWrite(int canfd ,struct can_frame frame)
{

#ifdef debug

    qDebug()<<"************ DataWrite **************";
    qDebug()<<"frame.can_id  = "<<frame.can_id;
    qDebug()<<"frame.can_dlc = "<<frame.can_dlc;
    for(int i= 0;i < 8;i++)
    {
        qDebug()<<"frame.data["<<i<<"] = "<<frame.data[i];
    }

#endif
    write(canfd, &frame, sizeof(struct can_frame));

}

//接收数据
int CanPort::dataRead(int canfd, can_frame &frame)
{
    int ret = read(canfd, &frame, sizeof(struct can_frame));
    return ret;
}
