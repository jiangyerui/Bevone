#include "calculanode.h"
#include <QDebug>
#include <QProcess>
#define DEBUG
#define CANLED 20

CalculaNode::CalculaNode(QObject *parent) : QObject(parent)
{

    m_passTime_first = true;
    m_passTime_first_count = 0;

    m_db = new MySqlite;
    m_passTime =  m_db->getPollTime();
    //    qDebug()<<"m_passTime = "<<m_passTime;
    //    m_passTime=70;//带128个探测器时用本数据
    //    m_passTime=32;//带60个探测器时用本数据

    //更新节点数量
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->start(TIMER);

    m_ledtimer = new QTimer;
    connect(m_ledtimer,SIGNAL(timeout()),this,SLOT(slotLedTimeOut()));
    m_ledtimer->start(100);


    m_curSound = NORMAL;
    m_oldSoundFlag = false;
    m_ioFlag = 0;
    m_reError = 0;
    m_reAlarm = 0;
    m_reDropped = 0;
    m_reErrorFlag = 0;
    m_curNet = 1;
    m_soundFlag = false;
    m_selfCheckFlag = false;
    m_strSend.clear();

    m_gpio = new GpioControl;
    m_record = new Record;
    m_cmdFlag = false;

    //m_gsm = QtSMS::getqtsms();
    //m_gsm->OpenCom(QString("/dev/ttyUSB0"));


}

void CalculaNode::initFlag()
{
    m_ioFlag = 0;
    m_reError = 0;
    m_reAlarm = 0;
    m_reDropped = 0;
    m_reErrorFlag = 0;
    m_soundFlag = false;
    m_oldSoundFlag = false;

}

void CalculaNode::initVar(bool powerType)
{
    m_powerType = powerType;
}

void CalculaNode::calculationPage(uint curNet)//curNet：当前通道
{
    int regNum = 0;//实际节点数量
    mutex.lock();
    for(uint net = 1;net < netMax;net++)//netMax:通道最大值3
    {
        for(uint id = 1;id <= idMax;id++)//idMax：当前节点最大值
        {
            if(mod[curNet][id].used == true)//如果该节点存在
            {
                if(curNet == net)
                {
                    //                    qDebug()<<"node["<<regNum<<"] = "<<id;
                    node[regNum++] = id;//记录节点位置
                    node[regNum] = 0;
                }
            }
        }
    }
    mutex.unlock();
    //    qDebug()<<"*******************";
    //计算页数
    int countPage = 0;
    if(regNum < 480)//如果节点数小于60*8，就一页了
    {
        countPage = 1;
    }
    else//否则大于一页
    {
        for(int i = 1;i < PAGEMAX;i++)
        {
            if((regNum > 480*i)&&( regNum <= 480*(i+1)))
            {
                countPage = i+1;
            }
        }
    }

    if(regNum != 0)
    {
        //qDebug()<<"regNum="<<regNum<<"***countPage="<<countPage;
        emit sigNodePage(node,regNum,countPage);//node：当前节点的ID值,regNum:当前节点数,countPage：当前页数
    }
    else
        emit sigNodePage(node,0,countPage);
}

void CalculaNode::calculaNodeStatus(uint GPIOFlag)
{
    uint alarm = 0;
    uint error = 0;
    uint droped= 0;
    uint used  = 0;

    m_used[1][0] = 0;
    m_used[2][0] = 0;
    m_droped[1][0] = 0;
    m_droped[2][0] = 0;

    uint curTime = QDateTime::currentDateTime().toTime_t();
    //    qDebug()<<" *** calculaNodeStatus ***";

    //jiangstrat
    if(m_passTime_first_count<12){
        m_passTime_first_count++;
    }else{
        m_passTime_first = false;
    }
    //jiangend

    for(uint net = 1;net < netMax;net++)
    {
        for(uint id = 1;id <= idMax;id++)
        {
            if(mod[net][id].used == true)//当前节点存在
            {
                used++;
                m_used[net][0]++;

                if(mod[net][id].dropFlag == false)
                {
                    mod[net][id].dropTimes++;
//                    if(id>32&&id<41){
                        //                        qDebug()<<"mod["<<net<<"]["<<id<<"].dropTimes = "<<mod[net][id].dropTimes;
//                    }

                    //jiangsstart
                    //                    if(m_passTime_first && mod[net][id].dropTimes>10){
                    //                        mod[net][id].dropTimes = 0;
                    //                        mod[net][id].dropFlag = true;
                    //                        mod[net][id].normalFlag = false;
                    //                        mod[net][id].errorFlag  = false;
                    //                    }
                    if(mod[net][id].dropTimes > m_passTime )//当前节点掉线18
                    {
                        int canId = id%8==0?id/8:((id/8)+1);
                        if(canId==5){
                            //                            qDebug()<<"mod["<<net<<"]["<<canId<<"].dropFlag = true";
                        }

                        mod[net][id].dropTimes = 0;
                        mod[net][id].dropFlag = true;
                        mod[net][id].normalFlag = false;
                        mod[net][id].errorFlag  = false;
                    }
                    //jiangend
                    //                    if(mod[net][id].dropTimes > m_passTime )//当前节点掉线18
                    //                    {
                    //                        int canId = id%8==0?id/8:((id/8)+1);
                    //                        qDebug()<<"mod["<<net<<"]["<<canId<<"].dropFlag = true";
                    //                        mod[net][id].dropTimes = 0;
                    //                        mod[net][id].dropFlag = true;
                    //                        mod[net][id].normalFlag = false;
                    //                        mod[net][id].errorFlag  = false;
                    //                    }
                }

                //当前节点正常
                if(mod[net][id].normalFlag == TRUE)
                {
                    mod[net][id].alarmFlag = FALSE;
                    mod[net][id].errorFlag = FALSE;
                    mod[net][id].dropFlag  = FALSE;

                    m_db->delTemp(net,id,DROP);//删除掉线
                    m_db->delTemp(net,id,ALARM);//删除报警
                    m_db->delTemp(net,id,ERROR);//删除故障
                }
                //节点掉线
                if(mod[net][id].dropFlag == TRUE)
                {
                    if(mod[net][id].used==true){    //jiang20190603
                        droped++;
                        m_droped[net][0]++;

                        if(mod[net][id].insertDrop == FALSE)
                        {
                            mod[net][id].insertDrop = TRUE;

                            mod[net][id].insertError = false;
                            m_db->delTemp(net,id,ERROR);//删除故障
                            QString add = m_db->getNodeAddress(net,id);
                            QString type,value;

                            if(MODULE_CUR == mod[net][id].type)
                            {
                                type = tr("漏电");
                                value = QString::number(mod[net][id].alarmData);
                                m_db->insertAlarm(net,id,MODULE_CUR,DROP,"0",curTime,add);//插入历史报警
                            }
                            else if(MODULE_TEM == mod[net][id].type)
                            {
                                type = tr("温度");
                                value = QString::number(mod[net][id].alarmTem);
                                m_db->insertAlarm(net,id,MODULE_TEM,DROP,"0",curTime,add);//插入历史报警
                            }

                            m_db->insertTemp(net,id,DROP,curTime);//插入临时故障

                            if(m_db->getPrintStyle())
                            {
                                if(m_db->getPrintError())
                                {
                                    //qDebug()<<"getPrintError";
                                    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd/hh:mm:ss");
                                    m_record->connectPrint(QString::number(net),QString::number(id),type,tr("通讯故障"),value,time,add);
                                }
                            }
                        }
                    }else{
                        m_db->delTemp(net,id,DROP);//删除掉线
                    }
                }
                else
                {
                    m_db->delTemp(net,id,DROP);//删除掉线
                }

                //节点故障
                if(mod[net][id].errorFlag == TRUE)
                {

                    if(mod[net][id].used==true){    //jiang20190603
                        error++;

                        if(mod[net][id].insertError == FALSE)
                        {
                            mod[net][id].insertError = TRUE;

                            m_db->delTemp(net,id,DROP);//删除掉线

                            QString add = m_db->getNodeAddress(net,id);
                            QString type,value;
                            if(MODULE_CUR == mod[net][id].type)
                            {
                                type = tr("漏电");
                                value = QString::number(mod[net][id].alarmData);
                                m_db->insertAlarm(net,id,MODULE_CUR,ERROR,"0",curTime,add);
                            }
                            else if(MODULE_TEM == mod[net][id].type)
                            {
                                type = tr("温度");
                                value = QString::number(mod[net][id].alarmTem);
                                m_db->insertAlarm(net,id,MODULE_TEM,ERROR,"0",curTime,add);
                            }
                            m_db->insertTemp(net,id,ERROR,curTime);//插入临时故障

                            if(m_db->getPrintStyle())
                            {
                                if(m_db->getPrintError() )
                                {

                                    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd/hh:mm:ss");
                                    m_record->connectPrint(QString::number(net),QString::number(id),type,tr("模块故障"),value,time,add);
                                }
                            }
                        }
                    }else{
                        m_db->delTemp(net,id,ERROR);//删除掉线
                    }

                }else{
                    m_db->delTemp(net,id,ERROR);//删除掉线
                }

                //节点报警
                if(mod[net][id].alarmFlag == TRUE)
                {
                    alarm++;

                    if(mod[net][id].insertAlarm == FALSE)
                    {
                        mod[net][id].insertAlarm = TRUE;
                        m_db->delTemp(net,id,DROP); //删除掉线
                        m_db->delTemp(net,id,ERROR);//删除故障

                        QString add = m_db->getNodeAddress(net,id);
                        QString type,value;
                        if(MODULE_CUR == mod[net][id].type)
                        {
                            type = tr("漏电");
                            value = QString::number(mod[net][id].alarmData);
                            m_db->insertTemp(net,id,ALARM,curTime);//插入临时
                            m_db->insertAlarm(net,id,MODULE_CUR,ALARM,value,curTime,add);//插入历史报警
                        }
                        else if(MODULE_TEM == mod[net][id].type)
                        {
                            type = tr("温度");
                            value = QString::number(mod[net][id].alarmTem);
                            m_db->insertTemp(net,id,ALARM,curTime);//插入临时
                            m_db->insertAlarm(net,id,MODULE_TEM,ALARM,value,curTime,add);//插入历史报警
                        }

                        if(m_db->getPrintStyle())
                        {
                            if(m_db->getPrintAlarm())
                            {
                                QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd/hh:mm:ss");
                                m_record->connectPrint(QString::number(net),QString::number(id),type,tr("报警"),value,time,add);
                            }
                        }
                    }
                    //发送报警短信
                    //说明:短信发送一次,复位后自重新发送
                    //if(g_smsType == true)
                    //{
                    //    //判断设备是否可用
                    //    if(mod[net][id].sent == FALSE)
                    //    {
                    //        send++;
                    //        mod[net][id].sent = TRUE;

                    //        QString typeStr;
                    //        switch (mod[net][id].type) {
                    //        case 2:
                    //            typeStr = "漏电报警";
                    //            break;
                    //        case 3:
                    //            typeStr = "温度报警";
                    //            break;
                    //        default:
                    //            break;
                    //        }
                    //        QString idStr = QString::number(id);
                    //        m_strSend +="地址:"+idStr+","+typeStr+";";

                    //        if(send == 6)
                    //        {
                    //            send = 0;
                    //            m_gsm->SendSms(m_strSend);
                    //            m_strSend.clear();
                    //        }
                    //    }
                    //}

                }
            }
        }
    }

    if(netMax == 3)
    {
        //重新注册can
        if(m_used[1][0] != 0 || m_used[2][0] != 0)
        {
            if( m_used[1][0] == m_droped[1][0] || m_used[2][0] == m_droped[2][0])
            {
                ::system("source /etc/profile");
            }
        }
    }
    else
    {
        //重新注册can
        if(m_used[1][0] != 0)
        {
            if(m_used[1][0] == m_droped[1][0])
            {
                ::system("source /etc/profile");
            }
        }
    }

    //控制指示灯,主机自检时不检测
    if(m_selfCheckFlag == false)
    {
        dealLedAndSound(alarm,error,droped,used,GPIOFlag);
    }

}

void CalculaNode::dealLedAndSound(uint alarm, uint error, uint droped, uint used, uint GPIOFlag)
{
    Q_UNUSED(used)
    //报警指示灯
    if(alarm > 0)
    {
        m_gpio->writeGPIO(GpioControl::AlarmLed,"1");
        m_gpio->writeGPIO(GpioControl::OutControl_1,"1");
        m_gpio->writeGPIO(GpioControl::OutControl_2,"1");
    }
    else
    {
        m_gpio->writeGPIO(GpioControl::AlarmLed,"0");
        m_gpio->writeGPIO(GpioControl::OutControl_1,"0");
        m_gpio->writeGPIO(GpioControl::OutControl_2,"0");
    }
    //故障指示灯
    if(droped > 0 || error > 0 || GPIOFlag > 0)
    {
        m_gpio->writeGPIO(GpioControl::ErrorLed,"1");
    }
    else
    {
        m_gpio->writeGPIO(GpioControl::ErrorLed,"0");
    }



    //静音时有新的故障,报警发生
    if(alarm > m_reAlarm)
    {
        m_reAlarm  = alarm;
        m_curSound = ALARM;
        m_soundFlag = false;
        soundControl(ALARM,true);
        return;
    }
    else if(droped > m_reDropped )
    {
        if(m_curSound == ALARM)
        {
            m_curSound = ALARM;
            soundControl(ALARM,true);
        }
        else
        {
            m_curSound = ERROR;
            soundControl(ERROR,true);
        }
        m_reDropped = droped;
        m_soundFlag = false;
        return;
    }
    else if(error > m_reError)
    {
        if(m_curSound == ALARM)
        {
            m_curSound = ALARM;
            soundControl(ALARM,true);
        }
        else
        {
            m_curSound = ERROR;
            soundControl(ERROR,true);
        }
        m_reError   = error;
        m_soundFlag = false;
        return;
    }
    else if(GPIOFlag > m_ioFlag)
    {
        if(m_curSound == ALARM)
        {
            m_curSound = ALARM;
            soundControl(ALARM,true);
        }
        else
        {
            m_curSound  = ERROR;
            soundControl(ERROR,true);
        }
        m_ioFlag = GPIOFlag;
        m_soundFlag = false;
        return;
    }

    if(alarm < m_reAlarm)
    {
        m_reAlarm = alarm;
    }

    //当报警节点数目小于总数时
    if(droped < m_reDropped)
    {
        m_reDropped = droped;
        if(m_oldSoundFlag != m_soundFlag)
        {
            uint errorNum = error + GPIOFlag + droped +alarm;
            if(m_reErrorFlag == errorNum)
            {
                soundControl(NORMAL,true);
            }
        }
    }

    if(error < m_reError)
    {
        m_reError = error;

        if(m_oldSoundFlag != m_soundFlag)
        {
            uint errorNum = error + GPIOFlag + droped +alarm;
            if(m_reErrorFlag == errorNum)
            {
                soundControl(NORMAL,true);
            }
        }

    }

    if(GPIOFlag < m_ioFlag)
    {
        m_ioFlag = GPIOFlag;
        if(m_oldSoundFlag != m_soundFlag)
        {
            uint errorNum = error + GPIOFlag + droped +alarm;
            if(m_reErrorFlag == errorNum)
            {
                soundControl(NORMAL,true);
            }
        }
    }

    if(m_soundFlag == true)
    {
        m_reErrorFlag = error + GPIOFlag + droped + alarm;
    }

    //都是正常消音
    if(error == 0 && GPIOFlag == 0 && droped == 0 && alarm == 0)
    {
        m_curSound = NORMAL;
        soundControl(NORMAL,true);
    }






#ifdef DEBUG
    /*
    qDebug()<<"********************************";
    qDebug()<<"error        = "<<error;
    qDebug()<<"alarm        = "<<alarm;
    qDebug()<<"GPIOFlag     = "<<GPIOFlag;
    qDebug()<<"droped       = "<<droped;
    qDebug()<<"reErrorFlag  = "<<m_reErrorFlag;
    qDebug()<<"oldSoundFlag = "<<m_oldSoundFlag;
    qDebug()<<"m_soundFlag  = "<<m_soundFlag;
    qDebug()<<"m_reError    = "<<m_reError;
    qDebug()<<"m_reAlarm    = "<<m_reAlarm;
    qDebug()<<"m_reDropped  = "<<m_reDropped;
    */
#endif
}

void CalculaNode::soundControl(int soundType, bool soundSwitch)
{
    //qDebug()<<"soundSwitch = "<<soundSwitch;
    //qDebug()<<"soundType   = "<<soundType;

    if(soundSwitch == true)
    {
        if(soundType == ALARM)
        {
            m_gpio->controlSound(ALARM);
        }
        else if(soundType == ERROR)
        {
            m_gpio->controlSound(ERROR);
        }
        else if(soundType == NORMAL)
        {
            m_gpio->controlSound(NORMAL);
        }
    }
    else
    {
        m_gpio->controlSound(NORMAL);
    }
}

void CalculaNode::slotLedTimeOut()
{
    if(m_canFlag == true)
    {
        if(!led)
        {
            led = true;
            m_gpio->writeGPIO(GpioControl::CanLed,"1");
        }
        else
        {
            led = false;
            m_canFlag = false;
            m_gpio->writeGPIO(GpioControl::CanLed,"0");
        }
    }
    else
    {
        m_gpio->writeGPIO(GpioControl::CanLed,"0");
    }

}

void CalculaNode::slotTimeOut()
{
    //显示节点状态，计算机点个数，计算页面数量

    calculationPage(m_curNet);//m_curNet当前通道
    calculaNodeStatus(m_gpio->dealGPIO());

}

void CalculaNode::slotReceiveLed()
{

    m_canFlag = true;
}

void CalculaNode::setSound(bool flag)
{
    m_soundFlag = flag;
    m_oldSoundFlag = m_soundFlag;
    m_curSound  = NORMAL;
}

void CalculaNode::setCurNet(int curNet)
{
    m_curNet = curNet;
}

void CalculaNode::setSelfCheckFlag(bool flag)
{
    m_selfCheckFlag = flag;
}

