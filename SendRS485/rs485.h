#ifndef RS485_H
#define RS485_H
#include <QtGlobal>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <QList>

class MetaDate{
public:
    qint8 data[2];
};

class RS485
{
public:
    RS485();

    QList<MetaDate> m_dateList;
    int fd_RS485;
    int initRS485(const char *portName);
    void dataCRC16(quint8 *crcData, int size, quint8 *ret);
    int writeRS485(uchar * arg,uint num); //向串口写入数据
    int readRS485(uchar * arg,uint num);  //从串口读出数据
    void insertData(qint8 value);
    void sendData();
};

#endif // RS485_H
