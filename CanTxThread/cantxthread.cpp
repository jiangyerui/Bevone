#include "cantxthread.h"
#include "GlobalData/globaldata.h"
#include "QDateTime"
#include <QDebug>
CanTxThread::CanTxThread()
{

}

void CanTxThread::setCanConf(CanPort *can, int canFd, int net,uint pollTime)
{
    this->m_can   = can;
    this->m_net   = net;
    this->m_canfd = canFd;
    this->m_pollTime = pollTime;
}

void CanTxThread::run()
{
    struct can_frame canFrame;
    while(1)
    {
        for(uint i = 0;i< cmdMax;i++)
        {
            msleep(m_pollTime);
            if(exeCmd[m_net][i].needExe == 1)
            {
                canFrame = exeCmd[m_net][i].canFrame;
                m_can->dataWrite(m_canfd,canFrame);

                //处理检测状态，检测通讯时间和标志位
                if(exeCmd[m_net][i].canFrame.data[0] == CMD_SE_STATE && exeCmd[m_net][i].dropped == FALSE)
                {
                    exeCmd[m_net][i].dropped = TRUE;//设置为掉线
                    exeCmd[m_net][i].sendTime = QDateTime::currentDateTime().toTime_t();//记录发送时间
                }
                //处理复位命令，发送后立即删除
                if(exeCmd[m_net][i].canFrame.data[0] == CMD_SE_RESET)
                {
                    exeCmd[m_net][i].needExe = 0;
                    exeCmd[m_net][i].canFrame.can_id  = 0;
                    exeCmd[m_net][i].canFrame.can_dlc = 0;
                    for(int j = 0 ;j < 8;j++)
                    {
                        exeCmd[m_net][i].canFrame.data[j] = 0;
                    }
                }
            }
        }

    }
}

