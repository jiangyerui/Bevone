#ifndef GPIOCONTROL_H
#define GPIOCONTROL_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include "MySqlite/mysqlite.h"
#include "GpioChip/gpiochip.h"

#define	EXPORT_PATH	"/sys/class/gpio/export"	        // GPIO设备导出设备

#define	DEV_PATH	"/sys/class/gpio/gpio68/value"      // 输入输出电平值设备
#define	DIRECT_PATH	"/sys/class/gpio/gpio68/direction"	// GPIO输入输出控制设备
#define	    OUT		"out"
#define	    IN		"in"
#define	    GPIO	"68"	// GPIO3_4
#define	HIGH_LEVEL	"1"
#define	LOW_LEVEL	"0"


/*
            𝑮𝑷𝑰𝑶排列序号=(𝑩𝑨𝑵𝑲−𝟏)×𝟑𝟐+𝑵
在公式中BANK为GPIO引脚所在的BANK，N则为引脚在该个BANK中的序号。
P3.4为例，其BANK值为3，N值为4，因此排列序号为(3-1)*32+4=68。
控制输出
GPIO2_09    GPIO41    报警
GPIO2_10    GPIO42    故障
GPIO2_11    GPIO43    通讯
GPIO2_12    GPIO44    备电正常
GPIO2_13    GPIO45    备电故障
GPIO2_14    GPIO46    主电正常
GPIO2_15    GPIO47    主电故障
控制输入
GPIO2_08    GPIO40    复位信号高电位
GPIO5_09    GPIO137   主电信号
GPIO5_05    GPIO133   备电信号
GPIO3_04    GPIO68    备电短路
继电器K1,K2 高电平导通
GPIO4_17    GPIO113    继电器1
GPIO4_18    GPIO114    继电器2

继电器K1,K2 高电平导通
GPIO4_17    GPIO113    继电器1
GPIO4_18    GPIO114    继电器2


*/
class GpioControl : public QObject
{
    Q_OBJECT
public:
    explicit GpioControl(QObject *parent = 0);
    //
    enum LedName{
        MainPowerRed     = 0x01,
        MainPowerGreen   = 0x02,
        BackupPowerRed   = 0x03,
        BackupPowerGreen = 0x04,
        CanLed = 0x05,
        Buzz   = 0x06,
        ErrorLed = 0x07,
        AlarmLed = 0x08,
        OutControl_1 = 0x09,
        OutControl_2 = 0x0A,
        ErrorBuzz = 0x0B,
        AlarmBuzz = 0x0C
    };

    enum WriteGPIO{
        GPIO_off = 0x00,
        GPIO_on  = 0x01
    };
    //
    enum ReadGPIO{
        MainPower = 0x01,
        BackupBower= 0x02,
        BackupError= 0x03,
        Reset      = 0x04
    };

    int m_fd;
    MySqlite m_db;
    //write操作
    //主电正常
    GpioChip *m_ledMainNormal;
    //主电故障
    GpioChip *m_ledMainError;
    //备电正常
    GpioChip *m_ledBackNormal;
    //备电故障
    GpioChip *m_ledBackError;
    //报警
    GpioChip *m_ledAlarm;
    //故障
    GpioChip *m_ledError;
    //通讯
    GpioChip *m_ledCan;
    //蜂鸣器
    GpioChip *m_buzz;
    //蜂鸣器类型
    GpioChip *m_buzzType;
    //复位
    GpioChip *m_btnReset;
    //继电器1
    GpioChip *m_ledRelay_1;
    //继电器2
    GpioChip *m_ledRelay_2;
    //read操作
    //主电状态
    GpioChip *m_mainPower;
    //备电状态
    GpioChip *m_backupPower;
    //备电故障
    GpioChip *m_backupError;
    QTimer *m_timer;
    uint times;


    bool m_mainPowerDb;
    bool m_backupPowerDb;
    uint m_mainPowerTimes;
    uint m_backupPowerTimes;

    uint m_resetTimes;

    uint dealGPIO();
    bool dealReset();
    //初始化GPIO状态
    void defaultStatus();
    uint readGPIO(int name);
    void selfCheck();
    void initGPIOChip();
    void writeGPIO(int name, const char *flag);
    void controlSound(int type);

signals:


public slots:
    void slotTimeOut();
};

#endif // GPIOCONTROL_H
