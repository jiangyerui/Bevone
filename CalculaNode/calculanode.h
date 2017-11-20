#ifndef CALCULANODE_H
#define CALCULANODE_H

#include <QObject>
#include <QTimer>
#include "GlobalData/globaldata.h"
#include "GpioControl/gpiocontrol.h"
#include "MySqlite/mysqlite.h"
#include "Record/record.h"
//#include "SMS/SMS.h"

#define TIMER 1000
#define NODENUM 1200
#define PAGEMAX 30
#define PASSTIME  60

class QTimer;
class GpioControl;

class CalculaNode : public QObject
{
    Q_OBJECT
public:
    explicit CalculaNode(QObject *parent = 0);
    QTimer *m_timer;
    QTimer *m_ledtimer;
    GpioControl *m_gpio;
    MySqlite *m_db;
    Record *m_record;
//    QtSMS  *m_gsm;

    uint m_curNet;
    uint m_passTime;

    //报警,故障重新打开
    uint m_ioFlag;
    uint m_reError;
    uint m_reAlarm;
    uint m_reDropped;
    bool m_soundFlag;
    bool m_powerType;
    bool m_selfCheckFlag;
    int node[1024];
    QString m_center;
    QString m_strNum;
    QString m_strSend;
    bool m_cmdFlag;
    uint m_used[3][1];
    uint m_droped[3][1];


    bool led;
    bool m_canFlag;

    void initVar(bool powerType);
    void setSound(bool flag);
    void setCurNet(int curNet);
    void setSelfCheckFlag(bool flag);
    //int calculationNode(uint curNet);//计算网络节点个数
    void calculationPage(uint curNet);//计算总总页数
    void calculaNodeStatus(uint GPIOFlag);
    void dealLedAndSound(uint alarm , uint error, uint droped, uint used, uint GPIOFlag);
    void soundControl(int soundType, bool soundSwitch);

signals:
    void sigNode(int *node,int size);
    void sigBtnSound();
    void sigSetCountPageNum(int);
    void sigNodePage(int *node,int size,int pageCount);

public slots:
    void slotLedTimeOut();
    void slotTimeOut();
    void slotReceiveLed();

};

#endif // CALCULANODE_H
