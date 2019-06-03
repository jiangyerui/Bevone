#include "canmoudle.h"


//#define DataRead
//#define DataWrite
#define ALARMTIMES 1
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
    //    m_canRxTimer->start(50);
    m_canRxTimer->setInterval(50);
    m_canRxTimer->start();//jiang20190521更改接收时间间隔为100毫秒

    m_canTxTimer = new QTimer;
    connect(m_canTxTimer,SIGNAL(timeout()),this,SLOT(slotCanTxTimeOut()),Qt::DirectConnection);
    //    m_canTxTimer->start(20);
    m_canTxTimer->setInterval(500);
    m_canTxTimer->start();//jiang20190521更改询问时间间隔为100毫秒


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
        //ioctl(device, CAN_IOCSRATE, 20*1000);	// 波特率20K ,jiang20190520
        qDebug()<<"**** Open "<<canName<<" OK **** ";
    }
}

int CanMoudle::dataWrite(int canfd, can_frame frame)
{
#ifdef DataWrite
    /*
    qDebug()<<"************ DataWrite **************";
    qDebug()<<"frame.can_id  = "<<frame.can_id;
    qDebug()<<"frame.can_dlc = "<<frame.can_dlc;
    for(int i= 0;i < 8;i++)
    {
        qDebug()<<"frame.data["<<i<<"] = "<<frame.data[i];
    }
*/
#endif
    int ret = send(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);
    return ret;
}

int CanMoudle::dataRead(int canfd, can_frame &frame)
{
    int ret = recv(canfd, &frame, sizeof(struct can_frame),MSG_DONTWAIT);
    return ret;
}

bool CanMoudle::revcFrameData(int canId){
    struct can_frame frame;
    int revcTimes = 24; //
    while(dataRead(m_canfd,frame)==-1 && revcTimes>0){  //如果成功读8次
        revcTimes--;
    }

    if(frame.can_id==canId){
        //收到数据
        emit sigReceiveLed();
        lc_dealCanData(frame);
        qDebug()<<"revcFrameData(int canId) = true *** canId="<<canId;
        return true;
    }
    return false;

    //    if(dataRead(m_canfd,frame)!=-1){
    //        if(frame.can_id==canId){
    //            //收到数据
    //            emit sigReceiveLed();
    //            lc_dealCanData(frame);
    //            qDebug()<<"revcFrameData(int canId) = true *** canId="<<canId;
    //            return true;
    //        }else{
    //            return false;
    //        }
    //    }else{
    //        return false;
    //    }
}
void CanMoudle::slotCanRxTimeOut()
{
    struct can_frame frame;
    //    for(uint i = 0;i <= 70;i++)
    //    {
    //        if(dataRead(m_canfd,frame) > 0)
    //        {
    //            //收到数据
    //            emit sigReceiveLed();
    //            //            dealCanData(frame);
    //            lc_dealCanData(frame);//jiang20190521解析联创的探测器
    //        }
    //    }

    for(uint i = 0;i <= 70;i++)
        if(dataRead(m_canfd,frame) > 0)
        {
            //收到数据
            emit sigReceiveLed();
            //        uint canId = frame.can_id;
            //            qDebug()<<"dataRead(m_canfd,frame) > 0";
            lc_dealCanData(frame);//jiang20190521解析联创的探测器
        }
}

void CanMoudle::dealCanData(can_frame frame)
{
    /*
    int cmd = frame.data[CAN_CMD];
    quint16 alarmData,rtData,baseData,canId;
    canId = frame.can_id;


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

            if(frame.data[2] == 0x01)//如果当前报警
            {
                mod[m_net][canId].leakTimes++;//报警次数加一
                if(mod[m_net][canId].leakTimes > ALARMTIMES)//如果之前报警过
                {
                    mod[m_net][canId].leakTimes = 0;//本次报警次数清零
                    nodeStatus(m_net,canId,frame.data[2]);//并更新状态
                    if(mod[m_net][canId].alaDataLock == false)//如果没有锁定报警值，就锁定它
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
                if(mod[m_net][canId].tempTimes > ALARMTIMES)
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
                emit sigSuccess();
                break;
            case MODULE_TEM:
                mod[m_net][canId].alarmTemSet = frame.data[2];//报警数值
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
    */
}

//解析联创的探测器
void CanMoudle::lc_dealCanData(can_frame frame)
{

    //jiang start 20190521
    int can_id_temp;
    int can_id_type;
    quint16 alarmData,rtData,baseData,canId;
    alarmData  = 0;//高位数
    alarmData |= 0;   //低位数
    baseData =  0;
    can_id_temp = (frame.can_id-1)*8+frame.data[0];//计算每个通道的id地址
    mod[m_net][can_id_temp].dropTimes = 0;
//    mod[m_net][can_id_temp].dropFlag = false;
//    qDebug()<<"can_id_temp["<<can_id_temp<<"].dropTimes="<<mod[m_net][can_id_temp].dropTimes;
    for(int j=0;j<5;j++){//解析每个通道的数据

        can_id_type = frame.data[1];
        switch (can_id_type) {
        case 1://类型为电流
            mod[m_net][can_id_temp].type = MODULE_CUR;
            mod[m_net][can_id_temp].dropFlag = false;
            if(frame.data[2] == 0x02 || frame.data[2]==0x01)//如果本次探测器漏电报警 或主动上报
            {
                rtData  = frame.data[3]<<8;//高位数
                rtData |= frame.data[4];   //低位数
                mod[m_net][can_id_temp].leakTimes++;
                if(mod[m_net][can_id_temp].leakTimes > ALARMTIMES)//如果之前报警过了
                {
                    mod[m_net][can_id_temp].leakTimes = 0;
                    nodeStatus(m_net,can_id_temp,1);//设探测器为报警状态
                    if(mod[m_net][can_id_temp].alaDataLock == false)//报警锁打开
                    {
                        mod[m_net][can_id_temp].alaDataLock = true;//报警保持
                        mod[m_net][can_id_temp].alarmData = rtData;//报警数据赋值
                    }
                }
                mod[m_net][can_id_temp].rtData = rtData;
            }
            else if(frame.data[2]==0x00)//正常
            {
                //实时数据
                rtData  = frame.data[3]<<8;//高位数
                rtData |= frame.data[4];   //低位数
                mod[m_net][can_id_temp].rtData = rtData;
                nodeStatus(m_net,can_id_temp,0);
            }else if(frame.data[2]==0x03)//故障
            {
                rtData  = 0;
                mod[m_net][can_id_temp].rtData = rtData;
                nodeStatus(m_net,can_id_temp,2);
            }else if(frame.data[2]==0xF2)//如果是读设定值返回的数据
            {
                //报警设定值
                alarmData  = frame.data[3]<<8;//高位数
                alarmData |= frame.data[4];   //低位数
                mod[m_net][can_id_temp].baseData = baseData;
                mod[m_net][can_id_temp].alarmDataSet = alarmData;//报警设定值
            }

            break;
        case 2://类型为温度
            //mod[m_net][can_id_temp].used = true;
            //            mod[m_net][can_id_temp].temData = frame.data[4];//实时数据
            //            mod[m_net][can_id_temp].baseData = 0;
            //            mod[m_net][can_id_temp].alarmTemSet = frame.data[4];//报警数值
            //            mod[m_net][can_id_temp].alarmTemSet = 0;//报警数值
            mod[m_net][can_id_temp].type = MODULE_TEM;
            mod[m_net][can_id_temp].dropFlag = false;
            if(frame.data[2] == 0x01 || frame.data[2]==0x02)//如果温度报警 或者主动上报
            {
                mod[m_net][can_id_temp].tempTimes++;
                if(mod[m_net][can_id_temp].tempTimes > ALARMTIMES)
                {
                    mod[m_net][can_id_temp].tempTimes = 0;
                    nodeStatus(m_net,can_id_temp,1);
                    if(mod[m_net][can_id_temp].alaTemLock == false)
                    {
                        mod[m_net][can_id_temp].alaTemLock = true;
                        mod[m_net][can_id_temp].alarmTem = frame.data[4];
                    }
                }
                mod[m_net][can_id_temp].temData = frame.data[4];
            }
            else if(frame.data[2]==0x00)//如果正常
            {
                mod[m_net][can_id_temp].temData = frame.data[4];
                nodeStatus(m_net,can_id_temp,0);
            }else if(frame.data[2]==0x03)//如果故障
            {
                mod[m_net][can_id_temp].temData = 0;
                nodeStatus(m_net,can_id_temp,2);
            }else if(frame.data[2]==0xF2)//如果是读设定值返回的数据
            {
                //报警设定值
                //                alarmData  = frame.data[3]<<8;//高位数
                //                alarmData |= frame.data[4];   //低位数
                //                mod[m_net][can_id_temp].baseData = baseData;
                mod[m_net][can_id_temp].alarmTemSet = frame.data[4];//报警设定值
            }
            break;
        case 0://如果未配接
//            mod[m_net][can_id_temp].dropFlag = true;
            mod[m_net][can_id_temp].used = false;
            break;
        default:
            break;
        }
    }

    //jiang end 20190521

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
//读取探测器的设定值，直到成功

bool CanMoudle::readSetVal(int net ,int index){

    m_canTxTimer->stop();
    bool flag = false;
    bool isRead = false;
    int count = 0 ;
    int readCount = 0;
    int canIdTemp = 0;
    int type = 0;
    uint alarmDataSet = 0;
    uint alarmTemSet = 0;
    //发送帧
    struct can_frame sendFrame;
    sendFrame.can_id  = index;
    sendFrame.can_dlc = 2;
    sendFrame.data[0] = 0xF2;
    sendFrame.data[1] = 0x55;
    //接收帧
    struct can_frame recvFrame;
    if(dataWrite(m_canfd,sendFrame)>0){
        //qDebug()<<"dataWrite(m_canfd,sendFrame)>0 = true";
        while(true){

            count++;
            for(int i=0;i<100;i++){//连续读取32次
                if(dataRead(m_canfd,recvFrame)!=-1){
                    //                    count++;
                    //                    qDebug()<<"dataRead(m_canfd,recvFrame)>0 = true";
                    if(recvFrame.data[2]==0xF2){
                        readCount++;
                        //                        qDebug()<<"recvFrame setVal success -- "+QString::number(index);
                        canIdTemp = (index-1)*8+recvFrame.data[0];//计算1-1024是哪个
                        type = recvFrame.data[1];
                        switch (type) {
                        case 1://如果是电流
                            alarmDataSet = recvFrame.data[3]<<8;
                            alarmDataSet = alarmDataSet|recvFrame.data[4];
                            mod[net][canIdTemp].alarmDataSet = alarmDataSet;
                            break;
                        case 2://如果是温度
                            alarmTemSet = recvFrame.data[4];
                            mod[net][canIdTemp].alarmTemSet = alarmTemSet;
                            break;
                        default:
                            break;
                        }
                        isRead = true;// 标记读取到了至少一次
                    }else{
                        //                        if(isRead == true){
                        //                            m_canTxTimer->start();
                        //                            return true;
                        //                        }
                    }
                    if(readCount>=8){//如果读取到了8次，表明读取完成，即可返回
                        m_canTxTimer->start();
                        return true;
                    }

                }else{
                    //                    qDebug()<<"dataRead(m_canfd,recvFrame)==-1)";
                }
            }

            //            qDebug()<<"count:"<<count;
            if(count>8){//如果读取了20*8次都没读取到，就返回false
                m_canTxTimer->start();
                return flag;
            }
        }
    }else{
        //        qDebug()<<"sendFrame setVal fail -- "+QString::number(index);
        m_canTxTimer->start();
        return flag;
    }
    //数据帧等待接收
    m_canTxTimer->start();
    return flag;
}
bool CanMoudle::sendFrameData(can_frame frame){
    for(int i=0;i<3;i++){
        int ret = dataWrite(m_canfd,frame);
        if(ret!=-1){
            return true;
        }
    }
    return false;
}
void CanMoudle::slotCanTxTimeOut()
{
    bool sendFlag = false;
    int recvTimes = 2;
    uint nodeIdTemp;

    //发送复位命令
    if(g_resetCmd[m_net] == true)
    {
        struct can_frame canFrame;
        canFrame.can_id  = 0x3FF;
        canFrame.can_dlc = 2;
        canFrame.data[0] = 0xFA;
        canFrame.data[1] = 0x55;
        for(uint k = 2;k<8;k++)
        {
            canFrame.data[k] = 0;
        }
        dataWrite(m_canfd,canFrame);
        g_resetCmd[m_net] = false;
    }
    else if(g_modSetCmd[m_net] == true)//处理设置探测器命令
    {
        //        struct can_frame canFrame;
        //        if(g_modLeak[m_net] == true)
        //        {

        //            canFrame.can_id  = leakData[CurNet].can_id;
        //            canFrame.can_dlc = leakData[CurNet].can_dlc;

        //            for(uint k = 0;k < 8;k++)
        //            {
        //                canFrame.data[k] = leakData[CurNet].data[k];
        //            }
        //            g_modLeak[m_net] = false;

        //        }
        //        else if(g_modTemp[m_net] == true)
        //        {
        //            canFrame.can_id  = tempData[CurNet].can_id;
        //            canFrame.can_dlc = tempData[CurNet].can_dlc;

        //            for(uint k = 0;k < 8;k++)
        //            {
        //                canFrame.data[k] = tempData[CurNet].data[k];
        //            }
        //            g_modTemp[m_net] = false;
        //        }
        //        dataWrite(m_canfd,canFrame);
    }
    else//发送巡检命令
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

            for(int i=0;i<3;i++){
                int ret = dataWrite(m_canfd,canFrame);
                //                qDebug()<<"ret="<<ret<<"***m_can_1="<<m_can_1;
                if(ret!=-1)
                    break;
            }


            //            sendFlag = sendFrameData(canFrame);
            //            if(sendFlag){
            //                while(!revcFrameData(m_can_1) && recvTimes>0){
            //                    sendFlag = false;
            //                    sendFlag = sendFrameData(canFrame);
            //                    recvTimes--;
            //                }
            //                if(recvTimes==0){   //如果没有收到本探测器的数据
            //                    for(int i=0;i<8;i++){
            //                        nodeIdTemp = ((m_can_1-1)*8)+i;
            //                        mod[m_net][nodeIdTemp].dropFlag = true;
            //                    }
            //                }
            //            }else{
            //                qDebug()<<"sendFrameData(canFrame) = false";
            //            }

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
            for(int i=0;i<3;i++){
                int ret = dataWrite(m_canfd,canFrame);
                //                qDebug()<<"ret="<<ret<<"***m_can_1="<<m_can_1;
                if(ret!=-1)
                    break;
            }
        }
    }
}


