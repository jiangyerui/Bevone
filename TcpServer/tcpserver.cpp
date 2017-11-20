#include "tcpserver.h"
#include "GlobalData/globaldata.h"

#define CMDALL 0x01
#define DATATYPE    0
#define DATANET     1
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
#define DATACMD     0x01

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
    QString dateStr = QDateTime::currentDateTime().toString("yyMMddhhmmss");
    NetMeta netMeta;
    netMeta.id  = id;
    netMeta.net = net;
    netMeta.data[DATATYPE] = type;
    netMeta.data[DATANET]  = net;
    netMeta.data[CANL] = id & 0XFF;
    netMeta.data[CANH] = id >> 8;
    netMeta.data[STS]  = sts;
    netMeta.data[CURL] = current & 0xFF;
    netMeta.data[CURH] = current >> 8;
    netMeta.data[TEM]  = tem;
    netMeta.data[YEAR]   = dateStr.left(2).toUInt();
    netMeta.data[MONTH]  = dateStr.mid(2,2).toUInt();
    netMeta.data[DAY]    = dateStr.mid(4,2).toUInt();
    netMeta.data[HOUR]   = dateStr.mid(6,2).toUInt();
    netMeta.data[MINUTE] = dateStr.mid(8,2).toUInt();
    netMeta.data[SECOND] = dateStr.right(2).toUInt();
    netMeta.data[UNUSED_1] = 0;
    netMeta.data[UNUSED_2] = 0;
    m_listData.append(netMeta);

}

void TcpServer::addData(uchar net, quint16 id, uchar type, uchar flag, quint16 leakCur,
                        quint16 leakSet, quint16 leakBase,quint16 temCur,quint16 temSet)
{
    NetMeta netMeta;
    netMeta.data[HEAD_AA] = 0xAA;
    netMeta.data[HEAD_00] = 0x00;
    netMeta.data[CMD]     = 0x02;
    netMeta.data[NET]     = net;
    netMeta.data[ADD_L]   = id & 0xFF;
    netMeta.data[ADD_H]   = id >> 8;
    netMeta.data[LONG]    = 10;
    netMeta.data[TYPE]    = type;
    netMeta.data[STATE]   = flag;
    netMeta.data[CUR_L]   = leakCur & 0xFF;
    netMeta.data[CUR_H]   = leakCur >> 8;
    netMeta.data[ALA_L]   = leakSet & 0xFF;
    netMeta.data[ALA_H]   = leakSet >> 8;
    netMeta.data[BASE_L]  = leakBase & 0xFF;
    netMeta.data[BASE_H]  = leakBase >> 8;
    netMeta.data[TEM_CUR] = temCur;
    netMeta.data[TEM_SET] = temSet;

    uchar crc = 0;
    for(int i = CMD;i<CRC;i++)
    {
        crc += netMeta.data[i];
    }
    netMeta.data[CRC] = crc;
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
            if(tempData[i+1] == 0x01)
            {
                //节点状态
                moduleStatus();
#ifdef  debug
                //qDebug()<<"tempData[0] = "<<(uchar)tempData[i];
                //qDebug()<<"tempData[1] = "<<(uchar)tempData[i+1];

#endif
                //qDebug("**************************");
                slotSendDataNew();
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

    qDebug()<<"dataSize : "<<dataSize;
    qDebug()<<"dataCount: "<<dataCount;
    netData[SERPID] = SER;
    netData[SERCMD] = DATACMD;
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
    int j = 1;
    for(int i = 6;i< m_listData.count()*SIZE+HEADDATA;i++)
    {

        qDebug()<<"netData["<<j++<<"]="<<netData[i];
        if(j == 17)
        {
            j = 1;
        }

    }
#endif

    m_tcpSocket->write((char*)netData,dataSize);
    netData = NULL;
    delete netData;
    dataClear();

}

void TcpServer::slotSendDataNew()
{
    qint64 dataSize = m_listData.count() * DATASIZE;
    uchar *netData = new uchar [dataSize];

    if(m_listData.count() == 0)
    {
        return;
    }

    for(int i = 0;i < m_listData.count();i++)
    {
        for(int j = 0;j<DATASIZE;j++)
        {
            netData[i * DATASIZE + j] = m_listData.at(i).data[j];
            //qDebug()<<"netData["<<j<<"]="<<netData[j];
        }
    }
    m_tcpSocket->write((char*)netData,dataSize);
    delete [] netData;
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
    for(uint net = 1;net < netMax;net++)
    {
        for(uint id = 1;id <= idMax;id++)
        {
            if(mod[net][id].used == true)
            {
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
                    modeSts = 9;
                }
                if(mod[net][id].alarmFlag == TRUE)
                {
                    modeSts = 1;
                }


                int type = mod[net][id].type;
                if(type == MODULE_CUR)
                {
                    qint16 leakCur  = mod[net][id].rtData;
                    qint16 leakSet  = mod[net][id].alarmDataSet;
                    qint16 leakBase = mod[net][id].baseData;

                    //qDebug()<<"leakCur  = "<<leakCur;
                    //qDebug()<<"leakSet  = "<<leakSet;
                    //qDebug()<<"leakBase = "<<leakBase;
                    //qDebug()<<"$$$$$$$$$$$$$$$$$$$$$$$$";
                    //addNetData(type,net,id,modeSts,data,0);
                    addData(net,id,type,modeSts,leakCur,leakSet,leakBase,0,0);
                }
                else if(type == MODULE_TEM)
                {

                    quint16 temSet = mod[net][id].alarmTemSet;
                    quint16 temCur = mod[net][id].temData;
                    //qDebug()<<"temCur = "<<temCur;
                    //qDebug()<<"temSet = "<<temSet;
                    //addNetData(type,net,id,modeSts,0,temp);
                    addData(net,id,type,modeSts,0,0,0,temCur,temSet);
                }
            }
        }
    }
}

void TcpServer::slotDisConnect()
{
    qDebug()<<"disconnect";
}

