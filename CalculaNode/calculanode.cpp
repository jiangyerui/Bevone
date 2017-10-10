#include "calculanode.h"
#include <QDebug>
#include <QProcess>
//#define DEBUG


CalculaNode::CalculaNode(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->start(TIMER);

    m_reError = 0;
    m_reAlarm = 0;
    m_reDropped = 0;
    m_curNet = 1;
    m_soundFlag = true;
    m_selfCheckFlag = false;
    m_strSend.clear();

    m_gpio = new GpioControl;
    m_db = new MySqlite;
    m_db->getSmsDetail(m_center,m_strNum);
    m_record = new Record;

    m_cmdFlag = false;

//    m_gsm = QtSMS::getqtsms();
//    m_gsm->OpenCom(QString("/dev/ttyUSB0"));


}

void CalculaNode::initVar(bool powerType)
{
    m_powerType = powerType;
}

int CalculaNode::calculationNode(int curNet)
{
    int regNum = 0;
    for(int i=0;i<IDNUM;i++)
    {
        node[i] = 0;
    }

    for(int net = 1;net < NETNUM;net++)
    {
        for(int id = 1;id < IDNUM;id++)
        {
            if(mod[net][id].used == true)
            {
                if(curNet == net)
                {
                    node[regNum++] = id;
                    node[regNum]   = 0;
                }
            }
        }
    }

    emit sigNode(node,regNum);
    return regNum;
}
int CalculaNode::calculationPage(int regNum)
{
    int countPage = 0;
    if(regNum < 40)
    {
        countPage = 1;
    }
    else
    {
        for(int i = 1;i < PAGEMAX;i++)
        {
            if((regNum > 40*i)&&( regNum <= 40*(i+1)))
            {
                countPage = i+1;
            }
        }
    }
    return countPage;
}

void CalculaNode::calculaNodeStatus(uint GPIOFlag)
{
    uint alarm = 0;
    uint error = 0;
    uint droped= 0;
    uint used  = 0;
//    uint send  = 0;
    m_used[1][0] = 0;
    m_used[2][0] = 0;
    m_droped[1][0] = 0;
    m_droped[2][0] = 0;

    for(uint net = 1;net < netMax;net++)
    {
        for(uint id = 1;id <= idMax;id++)
        {
            uint curTime  = QDateTime::currentDateTime().toTime_t();
            uint sendTime = exeCmd[net][id].sendTime;
            uint passTime = curTime - sendTime;

            if(mod[net][id].used == TRUE)//当前节点存在
            {
                used++;
                m_used[net][0]++;

                if(exeCmd[net][id].dropped == TRUE)//当前节点掉线
                {
                    if(passTime >= PASSTIME && mod[net][id].dropFlag == false )
                    {
                        mod[net][id].dropFlag = true;
                        mod[net][id].normalFlag = false;
                    }
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
                    droped++;
                    error++;
                    m_droped[net][0]++;
                    if(mod[net][id].insertDrop == FALSE)
                    {
                        mod[net][id].insertDrop = TRUE;
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
                                qDebug()<<"getPrintError";
                                QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd/hh:mm:ss");
                                m_record->connectPrint(QString::number(net),QString::number(id),type,tr("通讯故障"),value,time,add);
                            }
                        }
                    }
                }
                else
                {
                    m_db->delTemp(net,id,DROP);//删除掉线
                }
                //节点故障
                if(mod[net][id].errorFlag == TRUE)
                {
                    error++;
                    if(mod[net][id].insertError == FALSE)
                    {
                        mod[net][id].insertError = TRUE;
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
//                    if(g_smsType == true)
//                    {
//                        //判断设备是否可用
//                        if(mod[net][id].sent == FALSE)
//                        {
//                            send++;
//                            mod[net][id].sent = TRUE;

//                            QString typeStr;
//                            switch (mod[net][id].type) {
//                            case 2:
//                                typeStr = "漏电报警";
//                                break;
//                            case 3:
//                                typeStr = "温度报警";
//                                break;
//                            default:
//                                break;
//                            }
//                            QString idStr = QString::number(id);
//                            m_strSend +="地址:"+idStr+","+typeStr+";";

//                            if(send == 6)
//                            {
//                                send = 0;
//                                m_gsm->SendSms(m_strSend);
//                                m_strSend.clear();
//                            }
//                        }
//                    }

                }
#ifdef DEBUG
                qDebug()<<"*********************************";
                qDebug()<<"exeCmd dropped  = "<<exeCmd[net][id].dropped;
                qDebug()<<"passTime        = "<<passTime;
                qDebug()<<"mod  normalFlag = "<<mod[net][id].normalFlag;
                qDebug()<<"mod    dropFlag = "<<mod[net][id].dropFlag;
                qDebug()<<"mod   errorFlag = "<<mod[net][id].errorFlag;
#endif
            }
        }
    }

    if(netMax == 3)
    {
        //重新注册can
        if(m_used[1][0] == m_droped[1][0] || m_used[2][0] == m_droped[2][0])
        {
            ::system("source /etc/profile");
        }
    }
    else
    {
        //重新注册can
        if(m_used[1][0] == m_droped[1][0])
        {
            ::system("source /etc/profile");
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
    //通讯指示灯,节点掉线个数与总数相同
    if(used == droped)
    {
        m_gpio->writeGPIO(GpioControl::CanLed,"0");
    }
    else
    {
        m_gpio->writeGPIO(GpioControl::CanLed,"1");
    }
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
    if(error > 0 || GPIOFlag > 0)
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
        m_reAlarm = alarm;
        m_soundFlag = true;
        soundControl(ALARM,true);
    }
    else
    {
        //如果当前有报警的,新的故障也是发出报警声音
        if(droped > m_reDropped || error > m_reError)
        {
            if(alarm > 0)
            {
                m_soundFlag = true;
                soundControl(ALARM,true);
            }
            else
            {
                m_soundFlag = true;
                soundControl(ERROR,true);
            }
            m_reError   = error;
            m_reDropped = droped;
        }
    }
    //当报警节点数目小于总数时
    if(alarm <= m_reAlarm)
    {
        m_reAlarm = alarm;
    }

    if(droped <= m_reDropped)
    {
        m_reDropped = droped;
    }

    if(error <= m_reError)
    {
        m_reError = error;
    }

    //声音控制
    if(alarm > 0)
    {
        soundControl(ALARM,m_soundFlag);
    }
    else if(error > 0 || GPIOFlag > 0 || droped > 0)
    {
        soundControl(ERROR,m_soundFlag);
    }
    else
    {
        soundControl(NORMAL,m_soundFlag);
    }

#ifdef DEBUG
    qDebug()<<"droped       = "<<droped;
    qDebug()<<"error        = "<<error;
    qDebug()<<"m_soundFlag  = "<<m_soundFlag;
    qDebug()<<"m_reError    = "<<m_reError;
    qDebug()<<"m_reAlarm    = "<<m_reAlarm;
    qDebug()<<"m_reDropped  = "<<m_reDropped;
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
            emit sigBtnSound();
        }
        else if(soundType == ERROR)
        {
            m_gpio->controlSound(ERROR);
            emit sigBtnSound();
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

void CalculaNode::slotTimeOut()
{
    qDebug()<<"slotTimeOut";
    //显示节点状态，计算机点个数，计算页面数量
    int nodeNum = calculationNode(m_curNet);
    int countPage = calculationPage(nodeNum);
    emit sigSetCountPageNum(countPage);
    if(m_powerType == true)
    {
        calculaNodeStatus(m_gpio->dealGPIO());
    }
    else
    {
        calculaNodeStatus(0);
    }

}

void CalculaNode::setSound(bool flag)
{
    m_soundFlag = flag;
}

void CalculaNode::setCurNet(int curNet)
{
    m_curNet = curNet;
}

void CalculaNode::setSelfCheckFlag(bool flag)
{
    m_selfCheckFlag = flag;
}

