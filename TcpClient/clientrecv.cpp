#include "clientrecv.h"
#include <QDebug>
#include <QThread>
#define ALLSTS 0x01
#define ONESTS 0x04

#define debug


ClientRecv::ClientRecv(QObject *parent): QObject(parent)
{

}

void ClientRecv::updateSocket(QTcpSocket *tcpSocket)
{
    m_tcpSocket = tcpSocket;
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReceiveData()));
}

void ClientRecv::slotReceiveData()
{
    QByteArray qba =m_tcpSocket->readAll();
    char *tempData =  qba.data();
    for(int i = 0;i<qba.size();i++)
    {
        if(tempData[i] == 0x0A)
        {
            if(tempData[i+1] == ALLSTS)
            {
#ifdef  debug
                qDebug()<<"tempData[0] = "<<(uchar)tempData[i];
                qDebug()<<"tempData[1] = "<<(uchar)tempData[i+1];
                qDebug("**************************");
#endif
                emit sigAllSts();
            }
            else if(tempData[i+1] == ONESTS)
            {
                emit sigOneSts();
            }
        }
    }

}


