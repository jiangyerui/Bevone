#include "clientsend.h"
#include <QDebug>
#include <QThread>
#include <stdlib.h>
#include <unistd.h>
#include <QtEndian>

#define CMDALL 0x01
#define TYPE    0
#define NET     1
#define CANL    2
#define CANH    3
#define STS     4
#define CURL    5
#define CURH    6
#define TEM     7

#define LEN     8

#define SER     0
#define CMD     1
#define SIZE    2
#define COUNT   3



#define debug

ClientSend::ClientSend(QObject *parent) : QObject(parent)
{

}

void ClientSend::updateSocket(QTcpSocket *tcpSocket)
{
    m_tcpSocket = tcpSocket;
    addNetData(2,1,12,0,500,0);
    addNetData(2,2,1,0,939,0);
}

void ClientSend::sendData(char *sendData, qint64 size)
{
    qint64 size_t = m_tcpSocket->write(sendData,size);
    if(size_t < 0)
    {
        return;
    }
}

void ClientSend::slotSendData()
{

    if(m_listData.count() == 0)
    {
        return;
    }

    qDebug()<<"m_listData.count = "<<m_listData.count();

    uchar v_size  = m_listData.count()*LEN+4;
    uchar v_count = m_listData.count();
    uchar *netData = new uchar [v_size];
    netData[SER] = 0x0B;
    netData[CMD] = 0X01;
    netData[SIZE]  = qToLittleEndian(v_size);
    netData[COUNT] = qToLittleEndian(v_count);

    for(int i = 0;i<v_count;i++)
    {
        for(int j = 0;j<LEN;j++)
        {
            netData[i*LEN+j+4] = m_listData.at(i).data[j];
        }
    }
#ifdef debug
    for(int i = 0;i< v_size;i++)
    {
        qDebug()<<"netData = "<<netData[i];
    }
#endif

    qint64 size = v_size;
    m_tcpSocket->write((char*)netData,size);
    delete []netData;
    m_listData.clear();
}


void ClientSend::addNetData(uchar type, uchar net, quint16 id, uchar sts, quint16 current, uchar tem)
{
    NetMeta netMeta;
    netMeta.id  = id;
    netMeta.net = net;
    netMeta.data[TYPE] = type;
    netMeta.data[NET]  = net;
    netMeta.data[CANL] = id & 0XFF;
    netMeta.data[CANH] = id >> 8;
    netMeta.data[STS]  = sts;
    netMeta.data[CURL] = current & 0xFF;
    netMeta.data[CURH] = current >> 8;
    netMeta.data[TEM]  = tem;
    m_listData.append(netMeta);
}

void ClientSend::delNetData(uchar net, quint16 id)
{

    for(int i = 0;i< m_listData.count();i++)
    {
        if(m_listData.at(i).net == net && m_listData.at(i).id == id)
        {
            m_listData.removeAt(i);
        }
    }
}


