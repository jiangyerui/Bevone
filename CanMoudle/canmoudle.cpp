#include "canmoudle.h"


//#define DataRead
//#define DataWrite

CanMoudle::CanMoudle(QObject *parent) : QObject(parent)
{

}

CanMoudle::CanMoudle(const char *canName, int net, uint pollTime)
{

    Q_UNUSED(pollTime)
    initCan(canName);

    m_can_1 = 0;
    m_can_2 = 0;
    m_id = 0;
    m_net = net;
    m_idNum = 0;


    m_canRxTimer = new QTimer;
    connect(m_canRxTimer,SIGNAL(timeout()),this,SLOT(slotCanRxTimeOut()),Qt::DirectConnection);
    m_canRxTimer->start(80);

    m_canTxTimer = new QTimer;
    connect(m_canTxTimer,SIGNAL(timeout()),this,SLOT(slotCanTxTimeOut()),Qt::DirectConnection);
    m_canTxTimer->start(30);

}

void CanMoudle::initCan(const char *canName)
{
    m_canfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(m_canfd == -1)
    {
        qDebug("socket error");
    }
    else
    {
        strcpy(m_ifr.ifr_name, canName );
        if(ioctl(m_canfd, SIOCGIFINDEX, &m_ifr))
        {
            qDebug()<<"ioctl error";
            return;
        }

        m_addr.can_family = PF_CAN;
        m_addr.can_ifindex = m_ifr.ifr_ifindex;
        if(bind(m_canfd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0)
        {
            qDebug()<<"bind error";
            return;
        }
        qDebug()<<"**** Open "<<canName<<" OK **** ";
    }
}

int CanMoudle::dataWrite(int canfd, can_frame frame)
{
#ifdef DataWrite

    qDebug()<<"************ DataWrite **************";
    qDebug()<<"frame.can_id  = "<<frame.can_id;
    qDebug()<<"frame.can_dlc = "<<frame.can_dlc;
    for(int i= 0;i < 8;i++)
    {
        qDebug()<<"frame.data["<<i<<"] = "<<frame.data[i];
    }

#endif
    int ret = send(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);
    return ret;
}

int CanMoudle::dataRead(int canfd, can_frame &frame)
{
    int ret = recv(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);
    return ret;
}

void CanMoudle::slotCanRxTimeOut()
{
    struct can_frame frame;
    for(uint i = 0;i <= 50;i++)
    {
        if(dataRead(m_canfd,frame) > 0)
        {
            //收到数据
            emit sigReceiveLed();
            dealCanData(frame);
        }
    }

}

void CanMoudle::dealCanData(can_frame frame)
{

    int cmd = frame.data[CAN_CMD];
    quint16 alarmData,rtData,baseData,canId;
    canId = frame.can_id;

#ifdef DataRead

    qDebug()<<"                                           ************ DataRead **************";
    qDebug()<<"                                               m_net  = "<<m_net;
    qDebug()<<"                                               canId  = "<<frame.can_id;
    qDebug()<<"                                               canLon = "<<frame.can_dlc;
    for(int i = 0;i<8;i++)
    {
        qDebug()<<"                                               data["<<i<<"] = "<<frame.data[i];
    }
    qDebug()<<"                                           ************************************";

#endif

    uint type;
    switch (cmd) {
    case CMD_RE_STATE:  //探测器回复状态

        mod[m_net][canId].dropTimes = 0;
        type = frame.data[1];
        switch (type) {
        case MODULE_CUR:
            //实时数据
            rtData  = frame.data[4]<<8;//高位数
            rtData |= frame.data[3];   //低位数
            //报警数值
            //取出高4位数
            alarmData = frame.data[6] >> 4;
            alarmData <<= 8;
            alarmData |= frame.data[5];   //低位数
            //固有漏电
            //取出低4位数
            baseData =  frame.data[6] & 0xF;
            baseData <<= 8;
            baseData |= frame.data[7];//低位数

            mod[m_net][canId].used = true;
            mod[m_net][canId].rtData = rtData;//
            mod[m_net][canId].baseData = baseData;
            mod[m_net][canId].alarmDataSet = alarmData;
            mod[m_net][canId].type = frame.data[1];

            if(frame.data[2] == 0x01)
            {
                mod[m_net][canId].leakTimes++;
                if(mod[m_net][canId].leakTimes > 2)
                {
                    mod[m_net][canId].leakTimes = 0;

                    nodeStatus(m_net,canId,frame.data[2]);

                    if(mod[m_net][canId].alaDataLock == false)
                    {
                        mod[m_net][canId].alaDataLock = true;
                        mod[m_net][canId].alarmData = rtData;
                    }
                }

            }
            else
            {
                nodeStatus(m_net,canId,frame.data[2]);
            }

            break;
        case MODULE_TEM:

            mod[m_net][canId].used = true;
            mod[m_net][canId].temData = frame.data[3];//实时数据
            mod[m_net][canId].baseData = 0;
            mod[m_net][canId].alarmTemSet = frame.data[5];//报警数值
            mod[m_net][canId].type = frame.data[1];

            if(frame.data[2] == 0x01)
            {
                mod[m_net][canId].tempTimes++;
                if(mod[m_net][canId].tempTimes > 2)
                {
                    mod[m_net][canId].tempTimes = 0;
                    nodeStatus(m_net,canId,frame.data[2]);
                    if(mod[m_net][canId].alaTemLock == false)
                    {
                        mod[m_net][canId].alaTemLock = true;
                        mod[m_net][canId].alarmTem = frame.data[3];;
                    }
                }
            }
            else
            {
                nodeStatus(m_net,canId,frame.data[2]);
            }
            break;
        default:
            break;
        }
        break;
    case CMD_RE_SET:    //探测器回复设置

        if(CurNet == m_net)
        {
            g_modSetCmd[m_net] = false;
            type = frame.data[1];
            switch (type) {
            case MODULE_CUR:
                //报警数值
                //取出高4位数
                alarmData = frame.data[3] << 8;
                alarmData |= frame.data[2];   //低位数
                //固有漏电
                //取出低4位数
                baseData =  frame.data[5] << 8;
                baseData |= frame.data[4];//低位数

                mod[m_net][canId].baseData = baseData;
                mod[m_net][canId].alarmDataSet = alarmData;
                //g_modLeak = false;
                emit sigSuccess();

                break;
            case MODULE_TEM:
                mod[m_net][canId].alarmTemSet = frame.data[2];//报警数值
                //g_modTemp[m_net] = false;
                emit sigSuccess();
                break;
            default:
                break;
            }
        }
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

void CanMoudle::nodeStatus(int net, uint id, char status)
{
    switch (status) {
    case 0://normal
        mod[net][id].normalFlag  = true;
        mod[net][id].alarmFlag   = false;
        mod[net][id].errorFlag   = false;
        mod[net][id].dropFlag    = false;
        mod[net][id].insertAlarm = false;
        mod[net][id].insertError = false;
        mod[net][id].insertDrop  = false;
        break;
    case 1://alarm
        mod[net][id].alarmFlag  = true;
        mod[net][id].normalFlag = false;
        mod[net][id].errorFlag  = false;
        mod[net][id].dropFlag   = false;
        mod[net][id].insertDrop = false;
        mod[net][id].insertError = false;

        break;
    case 2://error
        mod[net][id].errorFlag  = true;
        mod[net][id].normalFlag = false;
        mod[net][id].dropFlag   = false;
        mod[net][id].insertDrop = false;
        break;
    default:
        break;
    }
}

void CanMoudle::controlTimer(bool flag)
{
    if(flag == true)
    {
        m_canTxTimer->start();
        m_canRxTimer->start();
    }
    else
    {
        m_canRxTimer->stop();
    }
}

void CanMoudle::getNodeNum(Exe_Cmd exeCmd[NETNUM][CMDEXENUM])
{
    Q_UNUSED(exeCmd)
}

void CanMoudle::slotCanTxTimeOut()
{
    //发送复位命令
    if(g_resetCmd[m_net] == true)
    {
        struct can_frame canFrame;
        canFrame.can_id  = ALLID;
        canFrame.can_dlc = 1;
        canFrame.data[0] = CMD_SE_RESET;
        for(uint k = 1;k<8;k++)
        {
            canFrame.data[k] = 0;
        }

        dataWrite(m_canfd,canFrame);
        g_resetCmd[m_net] = false;
    }
    else if(g_modSetCmd[m_net] == true)//处理设置探测器命令
    {
        struct can_frame canFrame;
        if(g_modLeak[m_net] == true)
        {

            canFrame.can_id  = leakData[CurNet].can_id;
            canFrame.can_dlc = leakData[CurNet].can_dlc;

            for(uint k = 0;k < 8;k++)
            {
                canFrame.data[k] = leakData[CurNet].data[k];
            }
            g_modLeak[m_net] = false;

        }
        else if(g_modTemp[m_net] == true)
        {
            canFrame.can_id  = tempData[CurNet].can_id;
            canFrame.can_dlc = tempData[CurNet].can_dlc;

            for(uint k = 0;k < 8;k++)
            {
                canFrame.data[k] = tempData[CurNet].data[k];
            }
            g_modTemp[m_net] = false;
        }
        dataWrite(m_canfd,canFrame);
    }
    else
    {
        if(m_net == 1)
        {
            m_can_1++;
            if(m_can_1 == canCmd_1_List.count())
            {
                m_can_1 = 0;
            }
            struct can_frame canFrame;
            canFrame = canCmd_1_List.at(m_can_1);
            dataWrite(m_canfd,canFrame);

        }
        else if(m_net == 2)
        {
            m_can_2++;
            if(m_can_2 == canCmd_2_List.count())
            {
                m_can_2 = 0;
            }
            struct can_frame canFrame;
            canFrame = canCmd_2_List.at(m_can_2);
            dataWrite(m_canfd,canFrame);
        }
    }
}


