#include "canmoudle.h"


//#define DataRead
//#define DataWrite

CanMoudle::CanMoudle(QObject *parent) : QObject(parent)
{

}

CanMoudle::CanMoudle(const char *canName, int net, uint pollTime)
{

    initCan(canName);
    this->m_net = net;
    m_id = 0;
    m_canRxTimer = new QTimer;
    connect(m_canRxTimer,SIGNAL(timeout()),this,SLOT(slotCanRxTimeOut()),Qt::DirectConnection);
    m_canRxTimer->start(pollTime);

    m_canTxTimer = new QTimer;
    connect(m_canTxTimer,SIGNAL(timeout()),this,SLOT(slotCanTxTimeOut()),Qt::DirectConnection);
    m_canTxTimer->start(pollTime);

}

void CanMoudle::initCan(const char *canName)
{
    m_canfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(m_canfd == -1)
    {
        qDebug("socket");
    }
    else
    {
        strcpy(m_ifr.ifr_name, canName );
        if(ioctl(m_canfd, SIOCGIFINDEX, &m_ifr))
        {
            qDebug()<<"ioctl error";
        }

        m_addr.can_family = PF_CAN;
        m_addr.can_ifindex = m_ifr.ifr_ifindex;
        if(bind(m_canfd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0)
        {
            qDebug()<<"bind error";
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
    int ret = dataRead(m_canfd,frame);
    if(ret > 0)
    {
        dealCanData(frame);
    }
}

void CanMoudle::dealCanData(can_frame frame)
{
    int cmd = frame.data[CAN_CMD];
    quint16 alarmData,rtData,baseData,canId;
    canId = frame.can_id;

#ifdef DataRead

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
            //取出高4位数
            alarmData = frame.data[6] >> 4;
            alarmData <<= 8;
            alarmData |= frame.data[5];   //低位数
            //固有漏电
            //取出低4位数
            baseData =  frame.data[6] & 0xF;
            baseData <<= 8;
            baseData |= frame.data[7];//低位数

            mod[m_net][canId].used = TRUE;
            mod[m_net][canId].rtData = rtData;//
            mod[m_net][canId].baseData = baseData;
            mod[m_net][canId].alarmDataSet = alarmData;
            mod[m_net][canId].type = frame.data[1];

            nodeStatus(m_net,canId,frame.data[2]);

            if(frame.data[2] == ALARM && mod[m_net][canId].alaDataLock == false)
            {
                mod[m_net][canId].alaDataLock = true;
                mod[m_net][canId].alarmData = rtData;
            }
            break;
        case MODULE_TEM:

            mod[m_net][canId].used = true;
            mod[m_net][canId].temData = frame.data[3];//实时数据
            mod[m_net][canId].baseData = 0;
            mod[m_net][canId].alarmTemSet = frame.data[5];//报警数值
            mod[m_net][canId].type = frame.data[1];
            nodeStatus(m_net,canId,frame.data[2]);
            if(frame.data[2] == ALARM && mod[m_net][canId].alaTemLock == false)
            {
                mod[m_net][canId].alaTemLock = true;
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

void CanMoudle::nodeStatus(int net, uint id, char status)
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
        mod[net][id].insertDrop = false;
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
        qDebug()<<"*****************************m_canRxTimer->stop()";
        m_canRxTimer->stop();
    }
}

void CanMoudle::getNodeNum(Exe_Cmd exeCmd[NETNUM][CMDEXENUM])
{
    Q_UNUSED(exeCmd)
}

void CanMoudle::slotCanTxTimeOut()
{
    struct can_frame canFrame;

    m_id++;
    //发送复位命令
    if(g_resetCmd == true)
    {
        canFrame.can_id  = ALLID;
        canFrame.can_dlc = 1;
        canFrame.data[0] = CMD_SE_RESET;
        for(uint k = 1;k<8;k++)
        {
            canFrame.data[k] = 0;
        }

        dataWrite(m_canfd,canFrame);
        m_canTxTimer->stop();

    }
    else if(exeCmd[m_net][m_id].needExe == 1)
    {
        canFrame = exeCmd[m_net][m_id].canFrame;
        dataWrite(m_canfd,canFrame);

        //处理检测状态，检测通讯时间和标志位
        if(exeCmd[m_net][m_id].canFrame.data[0] == CMD_SE_STATE && exeCmd[m_net][m_id].dropped == FALSE)
        {
            exeCmd[m_net][m_id].dropped  = TRUE;//设置为掉线
            exeCmd[m_net][m_id].sendTime = QDateTime::currentDateTime().toTime_t();//记录发送时间
        }
        //处理复位命令，发送后立即删除
        if(exeCmd[m_net][m_id].canFrame.data[0] == CMD_SE_RESET)
        {
            exeCmd[m_net][m_id].needExe = 0;
            exeCmd[m_net][m_id].canFrame.can_id  = 0;
            exeCmd[m_net][m_id].canFrame.can_dlc = 0;
            for(int j = 0 ;j < 8;j++)
            {
                exeCmd[m_net][m_id].canFrame.data[j] = 0;
            }
        }
    }

    if(m_id == cmdMax)
        m_id = 0;
}


