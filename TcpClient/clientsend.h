#ifndef CLIENTSEND_H
#define CLIENTSEND_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QList>

class NetMeta{

public:
    qint16 id;
    qint16 net;
    uchar data[8];
};


class ClientSend : public QObject
{
    Q_OBJECT
public:
    explicit ClientSend(QObject *parent = 0);
    void updateSocket(QTcpSocket *tcpSocket);
    QTcpSocket *m_tcpSocket;
    QList<NetMeta> m_listData;
    void sendData(char *sendData,qint64 size);

    void addNetData(uchar type, uchar net, quint16 id, uchar sts, quint16 current, uchar tem);
    void delNetData(uchar net, quint16 id);

public slots:
    void slotSendData();
};

#endif // CLIENTSEND_H
