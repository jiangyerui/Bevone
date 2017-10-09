#include "tcpserver.h"
#include "GlobalData/globaldata.h"

#define CMDALL 0x01
#define TYPE    0
#define NET     1
#define CANL    2
#define CANH    3
#define STS     4
#define CURL    5
#define CURH    6
#define TEM     7
//未确定
#define YEAR    8
#define MONTH   9
#define DAY     10
#define HOUR    11
#define MINUTE  12
#define SECOND  13
#define UNUSED_1   14
#define UNUSED_2   15




#define SIZE    16



#define ALLSTS 0x01
#define ONESTS 0x04

#define SER     0x0B
#define CMD     0x01

#define SERPID      0
#define SERCMD      1
#define SIZE_LIT    2
#define SIZE_BIG    3
#define COUNT_LIT   4
#define COUNT_BIG   5
#define HEADDATA    6


//#define debug

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    m_tcpSocket = new QTcpSocket;
    m_tcpServer = new QTcpServer;
    m_tcpServer->listen(QHostAddress::Any,5000);
    connect(m_tcpServer,SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
}


void TcpServer::dataClear()
{
    m_listData.clear();
}

void TcpServer::addNetData(uchar type, uchar net, quint16 id, uchar sts, quint16 current, uchar tem)
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

void TcpServer::delNetData(uchar net, quint16 id)
{
    for(int i = 0;i< m_listData.count();i++)
    {
        if(m_listData.at(i).net == net && m_listData.at(i).id == id)
        {
            m_listData.removeAt(i);
        }
    }
}

void TcpServer::slotReceiveData()
{
    QByteArray qba =m_tcpSocket->readAll();
    char *tempData =  qba.data();
    for(int i = 0;i<qba.size();i++)
    {
        if(tempData[i] == 0x0A)
        {
            if(tempData[i+1] == ALLSTS)
            {
                //节点状态
                moduleStatus();
#ifdef  debug
                qDebug()<<"tempData[0] = "<<(uchar)tempData[i];
                qDebug()<<"tempData[1] = "<<(uchar)tempData[i+1];
                qDebug("**************************");
#endif
                slotSendData();
            }
        }
    }
    tempData = NULL;
    delete tempData;
}

void TcpServer::slotSendData()
{
    uchar *netData = new uchar [m_listData.count()*SIZE+HEADDATA];

    quint16 dataSize  = m_listData.count()*SIZE+HEADDATA;
    quint16 dataCount = m_listData.count();
    netData[SERPID] = SER;
    netData[SERCMD] = CMD;
    netData[SIZE_LIT]  = dataSize & 0x0F;//取低八位
    netData[SIZE_BIG]  = dataSize >> 8;  //取高八位
    netData[COUNT_LIT] = dataCount & 0x0F;//取低八位
    netData[COUNT_BIG] = dataCount >> 8;  //取高八位


    if(m_listData.count() == 0)
    {
        return;
    }

    for(int i = 0;i<m_listData.count();i++)
    {
        for(int j = 0;j<SIZE;j++)
        {
            netData[i*SIZE+j+HEADDATA] = m_listData.at(i).data[j];
        }
    }

#ifdef debug
    for(int i = 0;i< m_listData.count()*SIZE+3;i++)
    {
        qDebug()<<"netData = "<<netData[i];
    }
#endif

    m_tcpSocket->write((char*)netData,dataSize);
    netData = NULL;
    delete netData;
    dataClear();
}

void TcpServer::slotNewConnection()
{
    qDebug()<<"********************";
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReceiveData()),Qt::DirectConnection);
    connect(m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisConnect()),Qt::DirectConnection);
}

void TcpServer::moduleStatus()
{
    dataClear();
    for(int net = 1;net <3;net++)
    {
        for(int id = 1;id < 1025;id++)
        {
            if(mod[net][id].used == false)
            {
                return;
            }

            uchar modeSts = 0;
            if(mod[net][id].normalFlag == TRUE)
            {
                modeSts = 0;
            }
            if(mod[net][id].errorFlag == TRUE)
            {
                modeSts = 2;
            }
            if(mod[net][id].dropFlag == TRUE)
            {
                modeSts = 4;
            }
            if(mod[net][id].alarmFlag == TRUE)
            {
                modeSts = 1;
            }

            int type = mod[net][id].type;
            if(type == MODULE_CUR)
            {
                uint data = mod[net][id].rtData;
                addNetData(type,net,id,modeSts,data,0);
            }
            else if(type == MODULE_TEM)
            {
                uint temp = mod[net][id].temData;
                addNetData(type,net,id,modeSts,0,temp);
            }
        }
    }
}

void TcpServer::slotDisConnect()
{
    qDebug()<<"disconnect";
}

