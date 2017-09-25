#ifndef MYPRINT_H
#define MYPRINT_H

#include <QDebug>
#include <QObject>
#include <QByteArray>
#include <QMessageBox>
#include <QTextCodec>
#include "LinuxCom/posix_qextserialport.h"
#include "MySqlite/mysqlite.h"


#define LF      0x0A //进纸命令
//初始化命令
#define ESC_AT   0x1B
#define ESC_AT2  0x40
//汉字模式命令
#define FS_ADD   0x1C
#define FS_ADD2  0x26

class MyPrint : public QObject
{
    Q_OBJECT
public:


    char lf[2];
//    char init[2];
//    char printCode[3];
//    char printinit[5];
//    char printChn[2];
    MySqlite m_db;
    QTextCodec *gbk;
    static MyPrint *print;
    Posix_QextSerialPort *myCom;

    void initCom();
    static MyPrint *getPrint();
    static void deletePrint();

    void printConnect(QString net, QString id, QString type, QString time,QString address);

signals:
    void showNoPageMessage();

public slots:
};

#endif // MYPRINT_H
