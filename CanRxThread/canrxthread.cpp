#include "canrxthread.h"
#include <QDebug>

//#define debug

CanRxThread::CanRxThread()
{
    m_netMax = 1;
    m_idMax  = 0;
}

void CanRxThread::run()
{
    struct can_frame frame;
    while (1) {
        msleep(m_pollTime);
        int ret = m_can->dataRead(m_canfd,frame);
        if(ret > 0)
        {
            dealCanData(frame);
        }
    }
}

void CanRxThread::setCanConf(CanPort *can,int canFd, int net,uint pollTime)
{
    this->m_can   = can;;
    this->m_net   = net;
    this->m_canfd = canFd;
    this->m_pollTime = pollTime;
}

void CanRxThread::dealCanData(struct can_frame frame)
{
    int cmd = frame.data[CAN_CMD];
    uint alarmData,rtData,baseData,canId;
    canId = frame.can_id;

#ifdef debug

    qDebug()<<"************ DataRead **************";
    qDebug()<<"m_net  = "<<m_net;
    qDebug()<<"canId  = "<<frame.can_id;
    qDebug()<<"canLon = "<<frame.can_dlc;
    for(int i = 0;i<8;i++)
    {
        qDebug()<<"data["<<i<<"] = "<<frame.data[i];

    }
    qDebug()<<"************************************";

#endif

    uint type;

    switch (cmd) {
    case CMD_RE_STATE:  //探测器回复状态

        exeCmd[m_net][canId].dropped = FALSE;
        exeCmd[m_net][canId].sendTime = QDateTime::currentDateTime().toTime_t();

        type = frame.data[1];
        switch (type) {
        case MODULE_CUR:
            //实时数据
            rtData  = frame.data[4]<<8;//高位数
            rtData |= frame.data[3];   //低位数
            //报警数值
            //取出低4位数
            alarmData = frame.data[6] & 0xF;
            alarmData <<= 8;
            alarmData |= frame.data[5];   //低位数
            //固有漏电
            //取出高4位数
            baseData =  frame.data[6] >> 4;
            baseData <<= 8;
            baseData |= frame.data[7];//低位数

            mod[m_net][canId].used = TRUE;
            mod[m_net][canId].rtData = rtData;//
            mod[m_net][canId].baseData = baseData;
            mod[m_net][canId].alarmDataSet = alarmData;
            mod[m_net][canId].type = frame.data[1];
            nodeStatus(m_net,canId,frame.data[2]);
            if(frame.data[2] == ALARM && mod[m_net][canId].alaDataLock == FALSE)
            {
                mod[m_net][canId].alarmData = rtData;
            }
            break;
        case MODULE_TEM:

            mod[m_net][canId].used = 1;
            mod[m_net][canId].temData = frame.data[3];//实时数据
            mod[m_net][canId].baseData = 0;
            mod[m_net][canId].alarmTemSet = frame.data[5];//报警数值
            mod[m_net][canId].type = frame.data[1];
            nodeStatus(m_net,canId,frame.data[2]);
            if(frame.data[2] == ALARM && mod[m_net][canId].alaTemLock == FALSE)
            {
                mod[m_net][canId].alarmTem = frame.data[3];;
            }

            break;
        default:
            break;
        }
        break;
    case CMD_RE_SET:    //探测器回复设置
        GlobalData::deleteCmd(m_net,canId,CMD_SE_SET);

        break;
    case CMD_RE_RESET:  //探测器回复复位
        GlobalData::deleteCmd(m_net,canId,CMD_SE_RESET);

        break;
    case CMD_RE_OFF:    //探测器回复单个静音关闭成功

        GlobalData::deleteCmd(m_net,canId,CMD_SE_OFF);

        break;
    case CMD_RE_ON:     //探测器回复单个静音开启成功
        GlobalData::deleteCmd(m_net,canId,CMD_SE_ON);
        break;
    default:
        break;
    }

}

void CanRxThread::nodeStatus(int net,uint id,char status)
{
    switch (status) {
    case 0://normal
        mod[net][id].normalFlag = true;
        mod[net][id].alarmFlag = false;
        mod[net][id].errorFlag = false;
        mod[net][id].dropFlag = false;
        mod[net][id].insertAlarm = false;
        mod[net][id].insertError = false;
        mod[net][id].insertDrop = false;

        break;
    case 1://alarm
        mod[net][id].alarmFlag  = true;
        mod[net][id].normalFlag = false;
        mod[net][id].dropFlag   = false;


        break;
    case 2://error
        mod[net][id].errorFlag  = true;
        mod[net][id].normalFlag = false;
        mod[net][id].dropFlag   = false;
        break;
    default:
        break;
    }

}


