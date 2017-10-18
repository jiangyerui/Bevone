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
    uchar data[16];
};

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

public slots:
    void slotReceiveData();
    void slotSendData();
    void slotNewConnection();
    void slotDisConnect();
};

#endif // TCPSERVER_H
