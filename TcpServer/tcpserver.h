#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDateTime>
class NetMeta{

public:
    qint16 id;
    qint16 net;
    uchar data[18];
};


#define HEAD_AA 0
#define HEAD_00 1
#define CMD     2
#define NET     3
#define ADD_H   4
#define ADD_L   5
#define LONG    6
#define TYPE    7
#define STATE   8
#define CUR_L   9
#define CUR_H   10
#define ALA_L   11
#define ALA_H   12
#define BASE_L  13
#define BASE_H  14
#define TEM_CUR 15
#define TEM_SET 16
#define CRC     17

#define DATASIZE    18
class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;
    QList<NetMeta> m_listData;
public:
    void dataClear();
    void moduleStatus();
    void delNetData(uchar net, quint16 id);
    void sendData(char *sendData,qint64 size);
    void addNetData(uchar type, uchar net, quint16 id, uchar sts, quint16 current, uchar tem);
    void addData(uchar net, quint16 id, uchar type, uchar flag, quint16 leakCur, quint16 leakSet, quint16 leakBase, quint16 temCur, quint16 temSet);


public slots:
    void slotReceiveData();
    void slotSendData();
    void slotSendDataNew();
    void slotNewConnection();
    void slotDisConnect();
};

#endif // TCPSERVER_H
