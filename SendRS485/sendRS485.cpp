#include "sendRS485.h"
#include <QThread>

SendRS485::SendRS485(QObject *parent) : QObject(parent)
{
    m_rs485 = new RS485();
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
}

int SendRS485::initRS485(const char *devName)
{
    int ret = m_rs485->initRS485(devName);
    if(ret == 1)
    {
        m_timer->start(1000);
        return 1;
    }
    return 0;
}

void SendRS485::slotTimeOut()
{
    quint8 modeSts = 0;
    for(int net = 1;net < 2;net++)
    {
        for(int id = 1;id < 1024;id++)
        {
            if(mod[net][id].used == true)
            {
                if(mod[net][id].normalFlag == TRUE)
                {
                    modeSts = 0;
                }
                if(mod[net][id].errorFlag  == TRUE)
                {
                    modeSts = 2;
                }
                if(mod[net][id].dropFlag   == TRUE)
                {
                    modeSts = 4;
                }
                if(mod[net][id].alarmFlag  == TRUE)
                {
                    modeSts = 1;
                }
                m_rs485->insertData(modeSts);
            }
        }
    }

    uchar data[6];
    m_rs485->readRS485(data,6);
    if(data[1] == 0x03)
    {
        m_rs485->sendData();
    }
}


