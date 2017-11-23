#include "serialhandle.h"

serial_data_buffer serial_rbuf;
serial_data_buffer serial_tbuf;

SerialHandle::SerialHandle(QObject *parent) : QObject(parent)
{
    initCom(QString("/dev/ttyUSB0"));

    times = 0;
    TX_timer = new QTimer ;
    connect(TX_timer,SIGNAL(timeout()),this,SLOT(slotTXTimeOut()),Qt::DirectConnection);
    RX_timer = new QTimer ;
    connect(RX_timer,SIGNAL(timeout()),this,SLOT(slotRXTimeOut()),Qt::DirectConnection);
    TX_timer->start(10);
    RX_timer->start(1000);
    memset(RX_data,0,100);
    memset(TX_data,0,15);
}

void SerialHandle::initSerial(int net)
{
    this->m_net = net;
}

bool SerialHandle::initCom(const QString &port)
{
    bool flag = false;
    m_port = new QSerialPort(port);
    if(m_port->isOpen())
    {
        m_port->close();
    }

    if(m_port->open(QIODevice::ReadWrite))
    {
        m_port->setBaudRate(QSerialPort::Baud115200);
        //数据位设置，我们设置为8位数据位
        m_port->setDataBits(QSerialPort::Data8);
        //奇偶校验设置，我们设置为无校验
        m_port->setParity(QSerialPort::NoParity);
        //停止位设置，我们设置为1位停止位
        m_port->setStopBits(QSerialPort::OneStop);
        //数据流控制设置，我们设置为无数据流控制
        m_port->setFlowControl(QSerialPort::NoFlowControl);
        flag = true;
        qDebug()<<"Open "<<port<<" OK !";
    }
    else
    {
        qDebug()<<"Open "<<port<<" failed !";
    }

    return flag;
}

void SerialHandle::dealData(uchar *data)
{
    uint type;

    uchar alarmData,rtData,baseData;

    int canId = (data[ADD_H]<<8) +data[ADD_L];

    switch (data[CMD]) {
    case CMD_RE_STATE:  //探测器回复状态

        //exeCmd[m_net][canId].dropped = FALSE;
        //exeCmd[m_net][canId].sendTime = QDateTime::currentDateTime().toTime_t();

        type = data[TYPE];
        switch (type) {
        case MODULE_CUR:
            //实时数据
            rtData = (data[CUR_H]<<8) | data[CUR_L];
            //报警数值
            alarmData = (data[ALA_H]<<8) | data[ALA_L];
            //固有漏电
            baseData = (data[BASE_H]<<8) | data[BASE_L];

            mod[m_net][canId].type = type;
            mod[m_net][canId].used = TRUE;
            mod[m_net][canId].rtData = rtData;
            mod[m_net][canId].baseData = baseData;
            mod[m_net][canId].alarmDataSet = alarmData;

            nodeStatus(m_net,canId,data[STATE]);

            if(data[STATE] == ALARM && mod[m_net][canId].alaDataLock == FALSE)
            {
                mod[m_net][canId].alarmData = rtData;
            }
            break;
        case MODULE_TEM:

            mod[m_net][canId].used = 1;
            mod[m_net][canId].temData = data[CUR_L];//实时数据
            mod[m_net][canId].baseData = 0;
            mod[m_net][canId].alarmTemSet = data[ALA_L];//报警数值
            mod[m_net][canId].type = type;
            nodeStatus(m_net,canId,data[STATE]);
            if(data[STATE] == ALARM && mod[m_net][canId].alaTemLock == FALSE)
            {
                mod[m_net][canId].alarmTem = data[CUR_L];
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

bool SerialHandle::crcData(uchar *data, uchar crc)
{
    //数组总个数
    int size = data[5] + _other;
    bool flag = false;
    uchar checkNum = 0;
    //数据叠加
    for(int j = 2; j < size+2; j++)
    {
        checkNum += data[j];
    }

    if(checkNum == crc)
    {
        flag = true;
    }
    return flag;
}

void SerialHandle::nodeStatus(int net, int id, char status)
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

int SerialHandle::readComData(char *data, qint64 maxSize)
{
    int ret = m_port->read(data,maxSize);
    return ret;
}

int SerialHandle::writeComData(const char *data, qint64 maxSize)
{
    int ret = m_port->write(data, maxSize);
    return ret;
}

void SerialHandle::dealBufData()
{
    int a = 0;
    int b = 0;
    int pt = 0;
    bool have_aa = false;
    uchar data[15];

    while(serial_rbuf.head != serial_rbuf.tail)
    {
        b = a;
        a = serial_rbuf.data[serial_rbuf.tail++];

        if(serial_rbuf.tail >= BUFNUM)
        {
            serial_rbuf.tail = 0;
        }
        //判断是否存在包头,跳出本次循环
        if((b == 0xaa) && (a == 0x00))
        {
            data[0] = 0xaa;
            data[1] = 0x00;
            pt = 2;
            continue;
        }
        data[pt++] = a;
        if(pt == 15)
        {
            //判断是否检验成功
            if(crcData(data,data[CRC]) == true)
            {
                have_aa = true;
            }
            else
            {
                have_aa = false;
            }
            pt = 0;
        }

        if(have_aa == true)
        {
            dealData(data);
        }
    }
}

//#define BUG

void SerialHandle::slotRXTimeOut()
{

    int dataNum = readComData((char *)RX_data,99);
#ifdef BUG
    qDebug()<<"dataNum = "<<dataNum;
#endif
    if((dataNum != -1)&&(dataNum < 100))
    {
        for(int i = 0;i < dataNum;i++)
        {
            m_b = m_a;
            m_a = RX_data[i];//串口读入数据
            serial_rbuf.data[serial_rbuf.head++] = m_a;//将数据缓存到
#ifdef BUG
            qDebug()<<"data = ["<<i<<"] = "<<RX_data[i];
#endif
            if(serial_rbuf.head >= BUFNUM)//判断数据包的长度
            {
                serial_rbuf.head = 0;
            }
        }
        dealBufData();
    }
}

void SerialHandle::slotTXTimeOut()
{
    times++;
    if(times == 1000)
    {
        times = 0;
    }

    for(int i = 0;i < D_lON;i++)
    {
        TX_data[i] = serialExeCmd[m_net][times].data[i];
    }
    writeComData((char *)TX_data,15);

}


