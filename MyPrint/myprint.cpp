#include "myprint.h"
#include <QDateTime>

#define POWER  4
#define BPOWER 5
#define ALARM  1
#define ERROR  2
#define CAN    3

MyPrint *MyPrint::print = NULL;

void MyPrint::initCom()
{
    gbk = QTextCodec::codecForName("GB18030");
    //ttymxc2
    myCom = new Posix_QextSerialPort("/dev/ttymxc1",QextSerialBase::Polling);
    //以读写方式打开串口
    bool opened = myCom->isOpen();
    if(opened)
    {
        qDebug()<<"opened!";
        myCom->close();
        qDebug()<<"closed!";
    }

    if(myCom->open(QIODevice::ReadWrite))
        qDebug()<<"open success!";
    else
        return;
    //波特率设置，我们设置为9600
    myCom->setBaudRate(BAUD9600);
    //数据位设置，我们设置为8位数据位
    myCom->setDataBits(DATA_8);
    //奇偶校验设置，我们设置为无校验
    myCom->setParity(PAR_NONE);
    //停止位设置，我们设置为1位停止位
    myCom->setStopBits(STOP_1);
    //数据流控制设置，我们设置为无数据流控制
    myCom->setFlowControl(FLOW_OFF);
    //延时设置，我们设置为延时500ms
    myCom->setTimeout(500);

    //进纸命令
    lf[0]=LF;
    lf[1]='\0';

//    //初始化
//    init[0] = 0x1B;
//    init[1] = 0x40;
//    //设置汉字字符模式
//    printinit[0]=0x1C;
//    printinit[1]=0x21;
//    printinit[2]=0x00;
//    //选择汉字模式
//    printChn[0]=0x1C;
//    printChn[1]=0x26;
//    //选择汉字代码系统
//    printCode[0] = 0x1C;
//    printCode[1] = 0x43;
//    printCode[2] = 0x00;

//    //myCom->write(init,3);
//    myCom->write(printinit,3);
//    myCom->write(printChn,2);
//    myCom->write(printCode,3);


}

MyPrint *MyPrint::getPrint()
{
    if(print==NULL)
    {
        print = new MyPrint();
    }
    return print;
}

void MyPrint::deletePrint()
{
    delete print;
}

void MyPrint::checkSelfPrint(QString nodeNum, QString alarmNum, QString errorNum, QString power, QString bpower)
{
    QByteArray byte;
    QString company =   tr("****北京北元安达电子有限公司****");
    byte = company.toLocal8Bit();
    myCom->write(byte);

    //自检完成
    QString compelet =  tr("自检完成");
    byte = compelet.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //节点总数
    QString modValue =  tr("节点总数 : ") + nodeNum;
    byte = modValue.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //故障数目
    QString modAddres = tr("故障数目 :") + alarmNum;
    byte = modAddres.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //报警数目
    QString modTime =   tr("报警数目 : ") + errorNum;
    byte = modTime.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //主电状态
    QString modStatus = tr("主电状态 : ") + power;
    byte = modStatus.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //备电状态
    QString printType = tr("备电状态 : ") + bpower;
    byte = printType.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}

void MyPrint::printConnect(QString net, QString id, QString type,QString status,QString value, QString time, QString address)
{
    QByteArray byte;
    QString company = tr("****北京北元安达电子有限公司****");
    byte = company.toLocal8Bit();
    myCom->write(byte);
    //打印地址
    QString modAddres = tr("地址 ：") + address;
    byte = modAddres.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印时间
    QString modTime = tr("时间 : ") + time;
    byte = modTime.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //数值
    QString modValue = tr("数值 : ") + value;
    byte = modValue.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //状态
    QString modStatus = tr("状态 : ") + status;
    byte = modStatus.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印类型
    QString printType = tr("类型 : ") + type;
    byte = printType.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印网络和地址
    QString modNetId=tr("通道 : ")+ net +tr("    节点 : ")+ id;
    byte = modNetId.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}
