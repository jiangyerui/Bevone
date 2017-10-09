#ifndef SYSTEMSET_H
#define SYSTEMSET_H

#include <QWidget>
#include <QTimer>
#include <time.h>
#include <QMessageBox>
#include <QDebug>
#include "MySqlite/mysqlite.h"
#include "GlobalData/globaldata.h"
#include "CanPort/canport.h"
#include "CanRxThread/canrxthread.h"
#include "CanTxThread/cantxthread.h"

#define PASTTIME  1000

/*    主要功能
 * 1.设置系统时间  ok
 * 2.设置密钥开关  ok
 * 3.设置主机编号  ok
 * 4.设置打印模式  ok
 * 5.更新用户密码  ok
 * 6.设置模块静音  ok
 * 7.设置模块蜂鸣  ok
 * 8.恢复出厂密码  ok
 * 9.设置轮询时间  ok
 *10.设置探测器参数 ok
 *11.烧录数据库文件 ok
 *
*/
namespace Ui {
class SystemSet;
}

class SystemSet : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSet(QWidget *parent = 0);
    ~SystemSet();

    MySqlite m_db;
    QTimer *m_pastTime;
    struct sysTime
    {
        int year;
        int month;
        int date;
        int hour;
        int minute;
        int second;
    };

    sysTime m_setTime;

    bool m_secretKeyFlag;
    QString m_serialNum;
    int m_userType;

    void initVar();
    void systemShow();
    void initConfigure();
    void initConnect();
    void setPrintType(bool type);
    void setSmsType(bool type);
    void setSerialNum(QString serialNum);

    void editClear();

private slots:

    void slotBtnUpdatePasswd();

    void slotBtnClose();
    void slotBtnSerialNum();
    void slotBtnPrintType();
    void slotBtnSysTime();
    void slotBtnStartSecret();
    void slotBtnStopSecret();
    void slotBtnRecoverPasswd();
    void slotPastTime();
    void slotPollTime();
    void slotBtnCopyDB();
    void slotBtnNodeData();

    void slotBtnStopSound();
    void slotBtnStartSound();

    void slotBtnSMS();

    void slotComboBoxPrint(int index);
    void slotBtnCal();


private:
    Ui::SystemSet *ui;
};

#endif // SYSTEMSET_H
