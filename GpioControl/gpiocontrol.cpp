#include "gpiocontrol.h"

#define POWER  4
#define BPOWERBREAK 5
#define BPOWERSHORT 6
#define MASTER      1 //主机
#define TIMES  10

//const char *exportPath = "/sys/class/gpio/export";
const char *out = "out";
const char *in  = "in";
const char *high= "1";
const char *low = "0";
/*
 *写操作引脚
*/
//报警
const char *ledAlarm = "41";
const char *ledAlarmValue = "/sys/class/gpio/gpio41/value";
const char *ledAlarmDirection = "/sys/class/gpio/gpio41/direction";
//故障
const char *ledError = "42";
const char *ledErrorValue = "/sys/class/gpio/gpio42/value";
const char *ledErrorDirection = "/sys/class/gpio/gpio42/direction";
//通讯
const char *ledCan = "43";
const char *ledCanValue = "/sys/class/gpio/gpio43/value";
const char *ledCanDirection = "/sys/class/gpio/gpio43/direction";
//备电正常
const char *ledBackupNormal = "44";
const char *ledBackupNormalValue = "/sys/class/gpio/gpio44/value";
const char *ledBackupNormalDirection = "/sys/class/gpio/gpio44/direction";
//备电故障
const char *ledBackupError = "45";
const char *ledBackupErrorValue = "/sys/class/gpio/gpio45/value";
const char *ledBackupErrorDirection = "/sys/class/gpio/gpio45/direction";
//主电正常
const char *ledMainNormal = "46";
const char *ledMainNormalValue = "/sys/class/gpio/gpio46/value";
const char *ledMainNormalDirection = "/sys/class/gpio/gpio46/direction";
//主电故障
const char *ledMainError = "47";
const char *ledMainErrorValue = "/sys/class/gpio/gpio47/value";
const char *ledMainErrorDirection = "/sys/class/gpio/gpio47/direction";
//蜂鸣器
const char *buzz = "115";
const char *buzzValue = "/sys/class/gpio/gpio115/value";
const char *buzzDirection = "/sys/class/gpio/gpio115/direction";
//声音类型
const char *buzzType = "116";
const char *buzzTypeValue = "/sys/class/gpio/gpio116/value";
const char *buzzTypeDirection = "/sys/class/gpio/gpio116/direction";
//继电器K1 高电平导通
const char *ledRelay_1 = "113";
const char *ledRelay_1Value = "/sys/class/gpio/gpio113/value";
const char *ledRelay_1Direction = "/sys/class/gpio/gpio113/direction";
//继电器K2 高电平导通
const char *ledRelay_2 = "114";
const char *ledRelay_2Value = "/sys/class/gpio/gpio114/value";
const char *ledRelay_2Direction = "/sys/class/gpio/gpio114/direction";
/*
 *读操作引脚
*/
//主电状态  高电平正常，低电平故障
const char *mainPower = "137";
const char *mainPowerValue = "/sys/class/gpio/gpio137/value";
const char *mainPowerDirection = "/sys/class/gpio/gpio137/direction";
//备电状态  低电平正常，高电平故障
const char *backupPower = "133";
const char *backupPowerValue = "/sys/class/gpio/gpio133/value";
const char *backupPowerDirection = "/sys/class/gpio/gpio133/direction";
//备电短路  备电高电平故障 高电平断路，低电平短路
const char *backupError = "68";
const char *backupErrorValue = "/sys/class/gpio/gpio68/value";
const char *backupErrorDirection = "/sys/class/gpio/gpio68/direction";

//复位信号
const char *btnReset = "40";
const char *btnResetValue = "/sys/class/gpio/gpio40/value";
const char *btnResetDirection = "/sys/class/gpio/gpio40/direction";


GpioControl::GpioControl(QObject *parent) : QObject(parent)
{
    times = 0;
    m_mainPowerDb = true;
    m_backupBreakDb = true;
    m_backupShortDb = true;
    m_mainPowerFlag =  true;
    m_backupBreakFlag = true;
    m_backupShortFlag = true;
    m_mainPowerTimes = 0;
    m_backupBreakTimes = 0;
    m_backupShortTimes = 0;
    this->initGPIOChip();
    defaultStatus();
    m_timer = new QTimer;
    m_timer->start(1000);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->stop();

}



void GpioControl::initGPIOChip()
{
    m_ledMainNormal = new GpioChip;
    m_ledMainNormal->initGPIOChip(ledMainNormal,ledMainNormalValue,ledMainNormalDirection);
    m_ledMainNormal->setState(out);

    m_ledMainError = new GpioChip;
    m_ledMainError->initGPIOChip(ledMainError,ledMainErrorValue,ledMainErrorDirection);
    m_ledMainError->setState(out);

    m_ledBackNormal = new GpioChip;
    m_ledBackNormal->initGPIOChip(ledBackupNormal,ledBackupNormalValue,ledBackupNormalDirection);
    m_ledBackNormal->setState(out);

    m_ledBackError = new GpioChip;
    m_ledBackError->initGPIOChip(ledBackupError,ledBackupErrorValue,ledBackupErrorDirection);
    m_ledBackError->setState(out);

    m_ledAlarm = new GpioChip;
    m_ledAlarm->initGPIOChip(ledAlarm,ledAlarmValue,ledAlarmDirection);
    m_ledAlarm->setState(out);

    m_ledError = new GpioChip;
    m_ledError->initGPIOChip(ledError,ledErrorValue,ledErrorDirection);
    m_ledError->setState(out);

    m_ledCan = new GpioChip;
    m_ledCan->initGPIOChip(ledCan,ledCanValue,ledCanDirection);
    m_ledCan->setState(out);


    m_buzz = new GpioChip;
    m_buzz->initGPIOChip(buzz,buzzValue,buzzDirection);
    m_buzz->setState(out);

    m_buzzType = new GpioChip;
    m_buzzType->initGPIOChip(buzzType,buzzTypeValue,buzzTypeDirection);
    m_buzzType->setState(out);

    m_btnReset = new GpioChip;
    m_btnReset->initGPIOChip(btnReset,btnResetValue,btnResetDirection);
    m_btnReset->setState(in);

    m_ledRelay_1 = new GpioChip;
    m_ledRelay_1->initGPIOChip(ledRelay_1,ledRelay_1Value,ledRelay_1Direction);
    m_ledRelay_1->setState(out);

    m_ledRelay_2 = new GpioChip;
    m_ledRelay_2->initGPIOChip(ledRelay_2,ledRelay_2Value,ledRelay_2Direction);
    m_ledRelay_2->setState(out);

    m_mainPower  = new GpioChip;
    m_mainPower->initGPIOChip(mainPower,mainPowerValue,mainPowerDirection);
    m_mainPower->setState(in);

    m_backupPower = new GpioChip;
    m_backupPower->initGPIOChip(backupPower,backupPowerValue,backupPowerDirection);
    m_backupPower->setState(in);

    m_backupError = new GpioChip;
    m_backupError->initGPIOChip(backupError,backupErrorValue,backupErrorDirection);
    m_backupError->setState(in);
}

void GpioControl::defaultStatus()
{
    writeGPIO(GpioControl::Buzz,high);
    writeGPIO(GpioControl::CanLed,high);
    writeGPIO(GpioControl::ErrorLed,low);
    writeGPIO(GpioControl::AlarmLed,low);
    writeGPIO(GpioControl::OutControl_1,low);
    writeGPIO(GpioControl::OutControl_2,low);
    writeGPIO(GpioControl::MainPowerGreen,high);
    writeGPIO(GpioControl::BackupPowerGreen,high);
}

void GpioControl::writeGPIO(int name, const char *flag)
{
    switch (name) {
    case 3://主电正常
        m_ledMainNormal->GPIOWrite(low);
        m_ledMainError->GPIOWrite(high);
        break;
    case 4://主电故障
        m_ledMainNormal->GPIOWrite(high);
        m_ledMainError->GPIOWrite(low);
        break;
    case 1://备电正常
        m_ledBackNormal->GPIOWrite(low);
        m_ledBackError->GPIOWrite(high);
        break;
    case 2://备电故障
        m_ledBackNormal->GPIOWrite(high);
        m_ledBackError->GPIOWrite(low);
        break;
    case 5://通讯
        m_ledCan->GPIOWrite(flag);
        break;
    case 6://蜂鸣器
        m_buzz->GPIOWrite(flag);
        break;
    case 7://故障
        m_ledError->GPIOWrite(flag);
        break;
    case 8://报警
        m_ledAlarm->GPIOWrite(flag);
        break;
    case 9://继电器1
        m_ledRelay_1->GPIOWrite(flag);
        break;
    case 10://继电器2
        m_ledRelay_2->GPIOWrite(flag);
        break;
    case 11://故障
        m_buzzType->GPIOWrite(flag);
        break;
    case 12://报警
        m_buzzType->GPIOWrite(flag);
        break;
    default:
        break;
    }
}

void GpioControl::controlSound(int type)
{
    //高电平消音
    //高电平故障
    //低电平报警
    switch (type) {
    case 0://正常
        writeGPIO(Buzz,"1");
        break;
    case 1://报警
        writeGPIO(Buzz,"0");
        writeGPIO(AlarmBuzz,"0");
        break;
    case 2://故障
        writeGPIO(Buzz,"0");
        writeGPIO(ErrorBuzz,"1");
        break;
    default:
        break;
    }
}




void GpioControl::slotTimeOut()
{
    times++;
    switch (times) {
    case 1:
        controlSound(2);
        writeGPIO(MainPowerRed,high);
        break;
    case 2:
        writeGPIO(MainPowerGreen,low);
        break;
    case 3:
        writeGPIO(BackupPowerRed,high);
        break;
    case 4:
        writeGPIO(BackupPowerGreen,low);
        break;
    case 5:
        writeGPIO(CanLed,low);
        break;
    case 6:
        controlSound(1);
        writeGPIO(CanLed,high);
        break;
    case 7:
        writeGPIO(ErrorLed,low);
        break;
    case 8:
        writeGPIO(ErrorLed,high);

        break;
    case 9:
        writeGPIO(AlarmLed,low);
        break;
    case 10:
        writeGPIO(AlarmLed,high);
        times = 0;
        m_timer->stop();
        writeGPIO(Buzz,high);
        break;
    default:
        break;
    }
}

uint GpioControl::readGPIO(int name)
{
    char i = 0;
    char buf[2]={'0','0'};

    switch(name)
    {
    case 1://主电
        m_mainPower->GPIORead(buf,mainPowerValue);
        break;
    case 2://备电
        m_backupPower->GPIORead(buf,backupPowerValue);
        break;
    case 3://备电短路
        m_backupError->GPIORead(buf,backupErrorValue);
        break;
    case 4://复位
        m_btnReset->GPIORead(buf,btnResetValue);
        break;
    default:
        break;
    }
    i = buf[0];
    return i;
}

void GpioControl::selfCheck()
{
    m_timer->start();
    //writeGPIO(Buzz,"0");
    controlSound(1);
}

uint GpioControl::dealGPIO()
{

    uint systemFlag = 0;
    //主电检测
    uint mainPower = readGPIO(MainPower);
    //qDebug()<<"mainPower : "<<mainPower;
    if(mainPower == '1')
    {
        m_mainPowerDb = true;
        m_mainPowerTimes = 0;
        m_mainPowerFlag = true;
        writeGPIO(MainPowerGreen,low);

        g_powerStatus = true;


    }
    else
    {
        m_mainPowerTimes++;
        if(m_mainPowerTimes > TIMES && m_mainPowerDb == true)
        {
            g_bpowerStatus = true;
            m_mainPowerDb = false;
            m_mainPowerFlag = false;
            uint curiTime = QDateTime::currentDateTime().toTime_t();
            m_db.insertAlarm(0,0,MASTER,POWER,"0",curiTime,tr("消防中控室"));
            writeGPIO(MainPowerRed,high);
        }
    }
    //备电检测

    uint backupPower = readGPIO(BackupBower);
    //qDebug()<<"backupPower : "<<backupPower;

    if(backupPower == '0')
    {
        m_backupBreakDb = true;
        m_backupShortDb = true;

        m_backupBreakTimes = 0;
        m_backupShortTimes = 0;

        m_backupBreakFlag = true;
        m_backupShortFlag = true;

        g_bpowerStatus = true;
        writeGPIO(BackupPowerGreen,high);

    }
    else
    {
        //备电短路，断路
        uint backError = readGPIO(BackupError);
        //qDebug()<<"backError : "<<backError;
        //qDebug()<<"m_backupBreakTimes :"<<m_backupBreakTimes;
        //qDebug()<<"m_backupShortTimes :"<<m_backupShortTimes;
        //qDebug()<<"m_backupBreakDb    :"<<m_backupBreakDb;
        //qDebug()<<"m_backupShortDb    :"<<m_backupShortDb;

        if(backError == '1')
        {
            m_backupBreakTimes++;
            if(m_backupBreakTimes > TIMES && m_backupBreakDb == true)
            {
                g_bpowerStatus = false;
                m_backupBreakDb = false;
                m_backupBreakFlag = false;
                uint curiTime = QDateTime::currentDateTime().toTime_t();
                m_db.insertAlarm(0,0,MASTER,BPOWERBREAK,"0",curiTime,tr("消防中控室"));

                writeGPIO(BackupPowerRed,high);
            }
        }
        else
        {
            m_backupShortTimes++;
            if(m_backupShortTimes > TIMES && m_backupShortDb == true)
            {
                g_bpowerStatus = false;
                m_backupShortDb = false;
                m_backupShortFlag = false;
                uint curiTime = QDateTime::currentDateTime().toTime_t();
                m_db.insertAlarm(0,0,MASTER,BPOWERSHORT,"0",curiTime,tr("消防中控室"));
                writeGPIO(BackupPowerRed,high);
            }
        }

    }

    //检测主电和备电标志，如果是故障，指示灯，声音为故障
    if(m_mainPowerFlag == false || m_backupBreakFlag == false || m_backupShortFlag == false)
    {
        systemFlag = 1;
    }

    return systemFlag;
}

bool GpioControl::dealReset()
{
    bool resetFlag = false;
    uint reset = readGPIO(Reset);
    //qDebug()<<"reset = "<<reset;
    if(reset == '0')
    {
        //        m_resetTimes++;
        //        if(m_resetTimes == 10)
        //        {
        resetFlag = true;
        //        }
    }
    return resetFlag;
}

