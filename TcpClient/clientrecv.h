#ifndef CLIENTRECV_H
#define CLIENTRECV_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
class ClientRecv : public QObject
{
    Q_OBJECT
public:
    explicit ClientRecv(QObject *parent = 0);

    void updateSocket(QTcpSocket *tcpSocket);
    QTimer *m_timer;
    QTcpSocket *m_tcpSocket;
    QTcpServer *tcpServer;

signals:
    void sigAllSts();
    void sigOneSts();

public slots:

    void slotReceiveData();

};

#endif // CLIENTRECV_H
