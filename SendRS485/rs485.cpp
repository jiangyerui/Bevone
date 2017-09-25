#include "rs485.h"
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QList>

static const quint8 table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const  quint8 table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

RS485::RS485()
{

}

int RS485::initRS485(const char *portName)
{
    //"/dev/ttySAC3"
    struct termios tio;
    if((fd_RS485 = open(portName,O_RDWR|O_NDELAY|O_NOCTTY)) < 0)
    {
        printf("+++++Open /ttymxc2 failed.+++++\n");
        return fd_RS485;
    }

    qDebug("+++++ttymxc2 open ok+++++\n");
    //设置波特率/8位数据位，1位停止位，无校验位/启动接收器(智能接收字源)/(本地连接，不具备数据机控制功能)忽略modem控制信号
    tio.c_cflag=B9600|CS8|CREAD|CLOCAL;
    //控制模式：在最后一个进程关闭设备之后，降低modem控制线(挂断)。
    tio.c_cflag&=~HUPCL;
    //本地模式
    tio.c_lflag=0;
    //输入模式：忽略帧错误和奇偶校验错
    tio.c_iflag=IGNPAR;
    //输出模式
    tio.c_oflag=0;
    //非canonical模式读的最小字符(no)
    tio.c_cc[VTIME]=0;
    //非canonical模式读时的延时，以十分之一秒为单位(no)
    tio.c_cc[VMIN]=0;
    //刷新输入队列，但是不能读
    tcflush(fd_RS485,TCIFLUSH);
    //激活新的配置
    tcsetattr(fd_RS485,TCSANOW,&tio);

    fcntl(fd_RS485,F_SETFL,FNDELAY);

    return 1;
}

void RS485::dataCRC16(quint8 *crcData,int size,quint8 *ret)
{
    quint8  crc_hi = 0xFF; /* high CRC byte initialized */
    quint8  crc_lo = 0xFF; /* low  CRC byte initialized */
    int i;             /* will index into CRC lookup */

    while (size --)
    {
        i = crc_hi ^ *crcData++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }
    ret[1] = crc_lo;
    ret[0] = crc_hi;

}

int RS485::writeRS485(uchar *arg, uint num)
{
    int  ret ;
    ret = ::write(fd_RS485, arg, num);
    return ret;
}

int RS485::readRS485(uchar *arg, uint num)
{
    int ret = 0;
    ret = ::read(fd_RS485, arg,num);
    return ret;
}
//数据存储
void RS485::insertData(qint8 value)
{
    MetaDate metaDate;
    metaDate.data[0] = 0x00;
    metaDate.data[1] = value;
    m_dateList.append(metaDate);

}

void RS485::sendData()
{
    quint32 i = 3;
    int j = 0;
    quint8 ret[2];
    quint32 byteCount = m_dateList.count() * 2 + 5;

    quint8 *tempData = new quint8[byteCount];

    tempData[0] = 0x01;
    tempData[1] = 0x03;
    tempData[2] = m_dateList.count() * 2;
    while(j < m_dateList.count())
    {
        tempData[i++] = m_dateList.at(j).data[0];
        tempData[i++] = m_dateList.at(j).data[1];
        j++;
    }
    //校验码
    dataCRC16(tempData,byteCount-2,ret);
    tempData[byteCount-2] = ret[0];
    tempData[byteCount-1] = ret[1];

//    for(quint32 n = 0;n < byteCount;n++)
//    {
//       qDebug()<<"tempData = "<<tempData[n];
//    }
//    qDebug()<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
    //写到串口

    writeRS485(tempData,byteCount);
    tempData = NULL;
    delete []tempData;
    m_dateList.clear();
}