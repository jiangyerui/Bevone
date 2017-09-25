#include "SMS.h"
#include <QDebug>
#include <QString>
QtSMS *QtSMS::qtsms = NULL;
QtSMS::QtSMS(QObject *parent) :
    QObject(parent)
{


}

QtSMS *QtSMS::getqtsms()
{
    if(qtsms==NULL)
    {
        qtsms = new QtSMS();
    }
    return qtsms;
}

bool QtSMS::OpenCom(const QString &name)
{
    bool flag = false;
    m_port = new QSerialPort(name);
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
    }

    return flag;

}

void QtSMS::SendSms(QString qStrSend)
{
    //设置信息内容
    m_port->write(qStrSend.toLocal8Bit());
}




