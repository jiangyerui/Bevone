#ifndef SERIALHANDLE_H
#define SERIALHANDLE_H

#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include "GlobalData/globaldata.h"


#define NET 3
#define ID  1025
#define D_lON 15

#define _other 4

#define HEAD_AA 0
#define HEAD_00 1
#define CMD     2
#define ADD_H   3
#define ADD_L   4
#define LONG    5
#define TYPE    6
#define STATE   7
#define CUR_L   8
#define CUR_H   9
#define ALA_L   10
#define ALA_H   11
#define BASE_L  12
#define BASE_H  13
#define CRC     14

struct serial_data_buffer
{
    uchar data[BUFNUM];
    uint  head;
    uint  tail;

};
extern serial_data_buffer serial_rbuf;
extern serial_data_buffer serial_tbuf;


class SerialHandle : public QObject
{
    Q_OBJECT
    //RX
public:
    explicit SerialHandle(QObject *parent = 0);

    void initSerial(int net);


    int m_a;
    int m_b;
    int m_net;
    QSerialPort *m_port;

    QTimer *RX_timer;
    QTimer *TX_timer;
    uchar TX_data[15];
    uchar RX_data[100];
    uint times;

    bool initCom(const QString &port);
    int writeComData(const char *data, qint64 len); //向串口写入数据
    int readComData(char *data, qint64 len);  //从串口读出数据

    void dealData(uchar *data);
    void dealBufData();
    bool crcData(uchar *data, uchar crc);
    void nodeStatus(int net, int id, char status);

signals:

public slots:
    void slotRXTimeOut();
    void slotTXTimeOut();

    //TX
public:

};

#endif // SERIALHANDLE_H
