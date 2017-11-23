#include "systemset.h"
#include "ui_systemset.h"
#include "MySqlite/mysqlite.h"

//#define RELEASE
#define MINUTE 60
#define HOUR   24

SystemSet::SystemSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemSet)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(0,0,800,480);

    ui->gBox_setSMS->hide();

    initVar();
    initConfigure();
    initConnect();
}

SystemSet::~SystemSet()
{
    delete ui;
}

void SystemSet::initVar()
{
    //大于0的数字
    QRegExp regExp("^[1-9][0-9]*$");
    ui->lineEdit_dayNum->setValidator(new QRegExpValidator(regExp, this));

//    QRegExp regExpPollTime("^(1[5-9][0-9])|([2-4][0-9][0-9])|500$");
//    ui->lineEdit_pollTime->setValidator(new QRegExpValidator(regExpPollTime, this));
    QRegExp regExpPollTime("^(1[5-9])|([2-8][0-9])$");
    ui->lineEdit_pollTime->setValidator(new QRegExpValidator(regExpPollTime, this));
    //只能是数字
    QRegExp regExpNumId("^([1-9][0-9]{1,2})|(101[0-9])|(102[0-4])|0$");//1-1024
    ui->lineEdit_id->setValidator(new QRegExpValidator(regExpNumId,this));
    QRegExp regExpNumNet("^[1-2]$");//1-2
    ui->lineEdit_net->setValidator(new QRegExpValidator(regExpNumNet,this));
    QRegExp regExpNumTem("^(([5-9][0-9])|(1[0-3][0-9])|140)$");//55-140
    ui->lineEdit_tem->setValidator(new QRegExpValidator(regExpNumTem, this));
    QRegExp regExpNumHave("([1-9][0-9]{1,2})|1000|0$");//0-1000
    ui->lineEdit_have->setValidator(new QRegExpValidator(regExpNumHave, this));
    QRegExp regExpNumAlarm("^([2-9][0-9][0-9])|(1[0-9][0-9][0-9])|2000$");//200-2000
    ui->lineEdit_alarm->setValidator(new QRegExpValidator(regExpNumAlarm, this));
    //code
    QRegExp regExpNum("^[1-9][0-9]{1,5}$");//不是0开头的6位数字
    ui->lineEdit_oldPasswd->setValidator(new QRegExpValidator(regExpNum, this));
    ui->lineEdit_newPasswd->setValidator(new QRegExpValidator(regExpNum, this));
    //serialNum
    QRegExp regExpDate("^((20[0-9]{2})|(0[0-9]{2}[1-9]))(0[1-9]|1[0-2])$");
    ui->lineEdit_date->setValidator(new QRegExpValidator(regExpDate, this));
    QRegExp regExpTimes("^[0-9][0-9][1-9]$");
    ui->lineEdit_times->setValidator(new QRegExpValidator(regExpTimes, this));
    QRegExp regExpSmallType("^[1-2][1-6]$");
    ui->lineEdit_smallType->setValidator(new QRegExpValidator(regExpSmallType, this));

    QRegExp regExpYear("^20[1-3][0-9]$");
    ui->lineEdit_year->setValidator(new QRegExpValidator(regExpYear, this));
    QRegExp regExpYeMonth("^[1-9]|(1[0-2])$");
    ui->lineEdit_month->setValidator(new QRegExpValidator(regExpYeMonth, this));
    QRegExp regExpDay("^([1-9])|([1-2][0-9])|(3[0-1])$");
    ui->lineEdit_day->setValidator(new QRegExpValidator(regExpDay, this));
    QRegExp regExpYHour("^([0-9])|(1[0-9])|(2[0-3])$");
    ui->lineEdit_hour->setValidator(new QRegExpValidator(regExpYHour, this));
    QRegExp regExpminute("^([0-9])|([1-5][0-9])$");
    ui->lineEdit_minute->setValidator(new QRegExpValidator(regExpminute, this));
    QRegExp regExpSecond("^([0-9])|([1-5][0-9])$");
    ui->lineEdit_second->setValidator(new QRegExpValidator(regExpSecond, this));


#ifdef RELEASE
    ui->lineEdit_pollTime->setVisible(false);
    ui->pBtn_pollTime->setVisible(false);
#endif

    m_pastTime = new QTimer;
    ui->pBtn_stopSound->hide();
    ui->pBtn_startSound->hide();
    ui->lineEdit_type->setEnabled(false);

}

void SystemSet::systemShow()
{
    show();
    ui->lb_version->hide();
    ui->lb_versionValue->hide();
    if(MySqlite::USER == m_userType)
    {

        ui->gBox_node->setEnabled(false);
        ui->gBox_other->setEnabled(false);
        ui->gBox_setTime->setEnabled(true);
        ui->gBox_setSMS->setEnabled(false);
        ui->gBox_setPrint->setEnabled(true);
        ui->gBox_setPasswd->setEnabled(false);
        ui->gBox_setScretkey->setEnabled(false);
        ui->gBox_setSerialNum->setEnabled(false);
        ui->lineEdit_pollTime->clear();


    }
    else if(MySqlite::ADMIN == m_userType)
    {
        ui->gBox_node->setEnabled(false);
        ui->gBox_other->setEnabled(false);
        ui->gBox_setSMS->setEnabled(true);
        ui->gBox_setTime->setEnabled(true);
        ui->gBox_setPrint->setEnabled(true);
        ui->gBox_setPasswd->setEnabled(true);
        ui->gBox_setScretkey->setEnabled(false);
        ui->gBox_setSerialNum->setEnabled(false);
        ui->lineEdit_pollTime->clear();
    }
    else if(MySqlite::SUPER == m_userType)
    {

        ui->gBox_node->setEnabled(true);
        ui->gBox_other->setEnabled(true);
        ui->gBox_setSMS->setEnabled(true);
        ui->gBox_setTime->setEnabled(true);
        ui->gBox_setPrint->setEnabled(true);
        ui->gBox_setPasswd->setEnabled(true);
        ui->gBox_setScretkey->setEnabled(true);
        ui->gBox_setSerialNum->setEnabled(true);

        ui->lb_version->show();
        ui->lb_versionValue->show();
        //ui->lb_versionValue->setText(m_db.getIVN());
        ui->lineEdit_pollTime->setText(QString::number(m_db.getPollTime()));
        ui->lineEdit_tem->setEnabled(false);
        ui->comboBox_type->setCurrentIndex(0);
    }
}

void SystemSet::initConfigure()
{
    //获取打印机配置
    setPrintType(m_db.getPrintStyle());
    //获取短信模式
    setSmsType(m_db.getSmsModel());


    //获取上锁状态,是否解锁
    m_secretKeyFlag = m_db.getSecretKeyFlag();
    if(m_secretKeyFlag)
    {
        m_pastTime->start(PASTTIME);
    }
    //获取序列号
    m_serialNum = m_db.getSerialNumber();
    setSerialNum(m_serialNum);

}

void SystemSet::initConnect()
{

    connect(ui->pBtn_quit,SIGNAL(clicked(bool)),this,SLOT(slotBtnClose()));
    connect(ui->pBtn_setType,SIGNAL(clicked(bool)),this,SLOT(slotBtnSerialNum()));
    connect(ui->pBtn_print,SIGNAL(clicked(bool)),this,SLOT(slotBtnPrintType()));
    connect(ui->pBtn_systemTime,SIGNAL(clicked(bool)),this,SLOT(slotBtnSysTime()));
    connect(ui->pBtn_start,SIGNAL(clicked(bool)),this,SLOT(slotBtnStartSecret()));
    connect(ui->pBtn_updatePasswd,SIGNAL(clicked(bool)),this,SLOT(slotBtnUpdatePasswd()));
    connect(ui->pBtn_stop,SIGNAL(clicked(bool)),this,SLOT(slotBtnStopSecret()));
    connect(m_pastTime,SIGNAL(timeout()),this,SLOT(slotPastTime()));
    connect(ui->pBtn_recoverPasswd,SIGNAL(clicked(bool)),this,SLOT(slotBtnRecoverPasswd()));
    connect(ui->pBtn_pollTime,SIGNAL(clicked(bool)),this,SLOT(slotPollTime()));

    connect(ui->pBtn_write,SIGNAL(clicked(bool)),this,SLOT(slotBtnWriteData()));
    connect(ui->pBtn_read,SIGNAL(clicked(bool)),this,SLOT(slotBtnReadNode()));

    connect(ui->pBtn_startSound,SIGNAL(clicked(bool)),this,SLOT(slotBtnStartSound()));
    connect(ui->pBtn_stopSound,SIGNAL(clicked(bool)),this,SLOT(slotBtnStopSound()));

    connect(ui->pBtn_SMS,SIGNAL(clicked(bool)),this,SLOT(slotBtnSMS()));

    connect(ui->comboBox_print,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComboBoxPrint(int)));
    connect(ui->pBtn_cal,SIGNAL(clicked(bool)),this,SLOT(slotBtnCal()));

    connect(ui->comboBox_type,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComboBoxType(int)));

}

void SystemSet::setPrintType(bool type)
{
    if(type)
    {
        //自动打印
        ui->comboBox_print->setCurrentIndex(1);
        g_printType = true;
        ui->checkBox_printError->setChecked(m_db.getPrintError());
        ui->checkBox_printAlarm->setChecked(m_db.getPrintAlarm());

    }
    else
    {
        //手动打印
        ui->comboBox_print->setCurrentIndex(0);
        g_printType = false;
        ui->checkBox_printAlarm->setEnabled(false);
        ui->checkBox_printError->setEnabled(false);

    }

}

void SystemSet::setSmsType(bool type)
{
    if(type)
    {
        //短信开启
        ui->comboBox_sms->setCurrentIndex(1);
        g_smsType = true;
    }
    else
    {
        //短信关闭
        ui->comboBox_sms->setCurrentIndex(0);
        g_smsType = false;
    }
}

void SystemSet::slotBtnPrintType()
{
    int index = ui->comboBox_print->currentIndex();
    if(index)
    {
        //自动打印
        g_printType = true;
        QMessageBox::information(NULL,tr("操作提示"), tr("已设置为自动打印模式！"),tr("关闭"));
    }
    else
    {
        //手动打印
        g_printType = false;
        QMessageBox::information(NULL,tr("操作提示"), tr("已设置为手动打印模式！"),tr("关闭"));
    }

    if(ui->checkBox_printError->checkState() == Qt::Checked)
        m_db.setPrintError(true);
    else
        m_db.setPrintError(false);

    if(ui->checkBox_printAlarm->checkState() == Qt::Checked)
        m_db.setPrintAlarm(true);
    else
        m_db.setPrintAlarm(false);;

    m_db.setPrintStyle(g_printType);
}

void SystemSet::slotBtnSysTime()
{

    if(ui->lineEdit_year->text().isEmpty()  ||
       ui->lineEdit_month->text().isEmpty() ||
       ui->lineEdit_day->text().isEmpty()   ||
       ui->lineEdit_hour->text().isEmpty()  ||
       ui->lineEdit_minute->text().isEmpty())
    {
        QMessageBox::information(NULL,tr("操作提示"), tr("日期/时间设置不能为空！"),tr("确定"));
        return;
    }
    int year,month,date,hour,minute,second;

    year  = ui->lineEdit_year->text().toInt();
    month = ui->lineEdit_month->text().toInt();
    date  = ui->lineEdit_day->text().toInt();


    switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        break;
    case 2:
        //是闰年
        if(((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
        {
            if(date > 29)
            {
                QMessageBox::information(NULL,tr("操作提示"), tr("日期设置错误！"),tr("确定"));
                return;
            }

        }
        else
        {
            if(date > 28)
            {
                QMessageBox::information(NULL,tr("操作提示"), tr("日期设置错误！"),tr("确定"));
                return;
            }
        }

        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if(date > 30)
        {
            QMessageBox::information(NULL,tr("操作提示"), tr("日期设置错误！"),tr("确定"));
            return;
        }
        break;
    default:
        break;
    }

    hour  = ui->lineEdit_hour->text().toInt();
    minute= ui->lineEdit_minute->text().toInt();

    if(ui->lineEdit_second->text().isEmpty() == true)
    {
        second = 0;
    }
    else
    {
        second = ui->lineEdit_second->text().toInt();
    }

    time_t time;
    struct tm p;
    p.tm_year = year-1900;
    p.tm_mon  = month-1;
    p.tm_mday = date;
    p.tm_hour = hour;
    p.tm_min  = minute;
    p.tm_sec  = second;
    time = mktime(&p);
    if(time < 0)
    {
        QMessageBox::information(NULL,tr("操作提示"), tr("时间修改失败！"),tr("确定"));
        return;
    }
    if(stime(&time) < 0)
    {
        QMessageBox::information(NULL,tr("操作提示"), tr("时间修改失败！"),tr("确定"));
    }
    else
    {
        ::system("hwclock -w");
        QMessageBox::information(NULL,tr("操作提示"), tr("时间修改成功！"),tr("确定"));
    }
}

void SystemSet::slotBtnStartSecret()
{
    QString dayStr = ui->lineEdit_dayNum->text();
    if(!dayStr.isEmpty())
    {
        ulong totleTime = dayStr.toULong() * HOUR * MINUTE;
        //qDebug()<<"totleTime = "<<totleTime;
        if(m_db.insertSecretKeyTime(totleTime,1))
        {
            m_pastTime->start(PASTTIME);
            m_db.setPastTime(0);
            QMessageBox::information(NULL,tr("操作提示"),tr("密钥设置生效！！！"),tr("关闭"));
        }
        else
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("密钥设置失败！！！"),tr("关闭"));
        }
    }
}

void SystemSet::slotBtnStopSecret()
{
    if(m_db.stopSecretKeyTime())
    {
        m_pastTime->stop();
        QMessageBox::information(NULL,tr("操作提示"),tr("密钥停止生效！！！"),tr("关闭"));
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("密钥停止失败！！！"),tr("关闭"));
    }

}

void SystemSet::slotBtnRecoverPasswd()
{
    if(m_userType < 3)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("当前操作权限受限！！！"),tr("关闭"));
        return;
    }
    else
    {
        m_db.recoverPasswd();
        QMessageBox::information(NULL,tr("操作提示"),tr("已经恢复出厂密码"),tr("关闭"));
    }

}

void SystemSet::slotPastTime()
{
    //判断是否启动加锁程序
    qulonglong ptime = m_db.getPastTime()+ 6;
    //qDebug()<<"ptime = "<<ptime;
    if(ptime > m_db.getTotleTime())
    {
        m_db.setSuccess(FALSE);
        m_db.insertSecretKeyTime(0,0);
        QProcess::execute("reboot");
    }
    else
    {
        m_db.setPastTime(ptime);
    }
}
//设置轮训时间
void SystemSet::slotPollTime()
{
    QString pollTime = ui->lineEdit_pollTime->text();
    if(pollTime.isEmpty())
    {
        QMessageBox::information(NULL,tr("设置轮循时间"),tr("时间参数不能设置为空!"),tr("关闭"));
    }
    else
    {

//        if(pollTime.toUInt() < 150)
//        {
//            QMessageBox::information(NULL,tr("设置轮循时间"),tr("轮循时间设定不能小于150ms"),tr("关闭"));
//            return;
//        }

        int ret = QMessageBox::information(NULL,tr("设置轮循时间"),tr("轮循时间设定为:")+pollTime+tr("ms"),tr("确定"),tr("取消"));
        if(ret == 0)
        {
            if(m_db.setPollTime(pollTime))
            {
                QMessageBox::information(NULL,tr("设置轮循时间"),tr("设定生效"),tr("确定"));
            }
        }
        else
        {
            ui->lineEdit_pollTime->clear();
            return;
        }
    }
}


void SystemSet::slotBtnReadNode()
{
    uint id  = ui->lineEdit_id->text().toUInt();
    uint net = ui->lineEdit_net->text().toUInt();

    if(mod[net][id].used == false)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("该节点不存在"),tr("关闭"));
        return;
    }

    ui->lineEdit_alarm->clear();
    ui->lineEdit_have->clear();
    ui->lineEdit_tem->clear();

    uint type = mod[net][id].type;
    if(type == MODULE_CUR)
    {
        uint leakSet = mod[net][id].alarmDataSet;
        uint leakBase = mod[net][id].baseData;
        ui->lineEdit_alarm->setText(QString::number(leakSet));
        ui->lineEdit_have->setText(QString::number(leakBase));
    }
    else if(type == MODULE_TEM)
    {
        uint temSet = mod[net][id].alarmTemSet;
        ui->lineEdit_tem->setText(QString::number(temSet));
    }

}

void SystemSet::slotBtnWriteData()
{

    QString idStr    = ui->lineEdit_id->text();
    QString netStr   = ui->lineEdit_net->text();

    int index = ui->comboBox_type->currentIndex() + 2;
    if(index == MODULE_CUR)
    {
        if(mod[netStr.toInt()][idStr.toInt()].type != MODULE_CUR)
        {
            QMessageBox::information(NULL,tr("错误提示"),tr("探测器与参数设定值不匹配！"),tr("关闭"));
            return ;
        }
        QString haveStr  = ui->lineEdit_have->text();
        QString alarmStr = ui->lineEdit_alarm->text();
        if(alarmStr.toUInt() < 200)
        {
            QMessageBox::information(NULL,tr("错误提示"),tr("漏电设定值不能小于200mA"),tr("关闭"));
            return;
        }

        for(int i = 0;i<8;i++)
        {
            leakData[netStr.toInt()].data[i] = 0;
        }
        leakData[netStr.toInt()].can_id  = idStr.toUInt();
        leakData[netStr.toInt()].can_dlc = 8;
        leakData[netStr.toInt()].data[0] = CMD_SE_SET;
        leakData[netStr.toInt()].data[1] = MODULE_CUR;
        leakData[netStr.toInt()].data[2] = alarmStr.toUInt() & 0xFF;//低八位;
        leakData[netStr.toInt()].data[3] = alarmStr.toUInt() >> 8;
        leakData[netStr.toInt()].data[4] = haveStr.toUInt() & 0xFF;
        leakData[netStr.toInt()].data[5] = haveStr.toUInt() >> 8;
        leakData[netStr.toInt()].data[6] = 0;
        leakData[netStr.toInt()].data[7] = 0;
        CurNet = netStr.toInt();
        g_modLeak[CurNet] = true;
        g_modSetCmd[CurNet] = true;
    }
    else if(index == MODULE_TEM)
    {
        if(mod[netStr.toInt()][idStr.toInt()].type != MODULE_TEM)
        {
            QMessageBox::information(NULL,tr("错误提示"),tr("探测器与参数设定值不匹配！"),tr("关闭"));
            return ;
        }

        QString temStr   = ui->lineEdit_tem->text();
        if(temStr.toUInt() < 55)
        {
            QMessageBox::information(NULL,tr("错误提示"),tr("温度设定值不能小于55℃"),tr("关闭"));
            return;
        }

        for(int i = 0;i<8;i++)
        {
            tempData[netStr.toInt()].data[i] = 0;
        }
        tempData[netStr.toInt()].can_id  = idStr.toUInt();
        tempData[netStr.toInt()].can_dlc = 8;
        tempData[netStr.toInt()].data[0] = CMD_SE_SET;
        tempData[netStr.toInt()].data[1] = MODULE_TEM;
        tempData[netStr.toInt()].data[2] = temStr.toUInt() & 0xFF;
        tempData[netStr.toInt()].data[3] = temStr.toUInt() >> 8;
        tempData[netStr.toInt()].data[4] = 0;
        tempData[netStr.toInt()].data[5] = 0;
        tempData[netStr.toInt()].data[6] = 0;
        tempData[netStr.toInt()].data[7] = 0;
        CurNet = netStr.toInt();
        g_modTemp[CurNet]   = true;
        g_modSetCmd[CurNet] = true;
    }
}

void SystemSet::slotBtnStopSound()
{
    uchar temp[1];
    QString idStr    = ui->lineEdit_id->text();
    QString netStr   = ui->lineEdit_net->text();
    temp[0] = CMD_SE_OFF;
    GlobalData::addCmd(netStr.toInt(),idStr.toInt(),temp[0],temp,1);
}

void SystemSet::slotBtnStartSound()
{
    uchar temp[1];
    QString idStr  = ui->lineEdit_id->text();
    QString netStr = ui->lineEdit_net->text();
    temp[0] = CMD_SE_ON;
    GlobalData::addCmd(netStr.toInt(),idStr.toInt(),temp[0],temp,1);
}
//设置短信开启关闭模式
void SystemSet::slotBtnSMS()
{
    int index = ui->comboBox_sms->currentIndex();
    if(index == 0)
    {
        //关闭短信功能
        if(m_db.setSmsModel(false))
        {
            g_smsType = false;
            QMessageBox::information(NULL,tr("提示"),tr("短信功能已经关闭!"),tr("关闭"));
        }
        else
        {
            QMessageBox::information(NULL,tr("提示"),tr("短信功能关闭失败!"),tr("关闭"));
        }
    }
    else
    {
        //开启短信功能
        if(m_db.setSmsModel(true))
        {
            g_smsType = true;
            QMessageBox::information(NULL,tr("提示"),tr("短信功能已经开启!"),tr("关闭"));
        }
        else
        {
            QMessageBox::information(NULL,tr("提示"),tr("短信功能开启失败!"),tr("关闭"));
        }
    }
}

void SystemSet::slotComboBoxPrint(int index)
{
    if(index == 0)
    {
        ui->checkBox_printError->setEnabled(false);
        ui->checkBox_printAlarm->setEnabled(false);
    }
    else if(index == 1)
    {
        ui->checkBox_printError->setEnabled(true);
        ui->checkBox_printAlarm->setEnabled(true);
    }
}

void SystemSet::slotComboBoxType(int index)
{
    if(index == 0)
    {
        ui->lineEdit_have->setEnabled(true);
        ui->lineEdit_alarm->setEnabled(true);
        ui->lineEdit_tem->setEnabled(false);
    }
    else
    {
        ui->lineEdit_have->setEnabled(false);
        ui->lineEdit_alarm->setEnabled(false);
        ui->lineEdit_tem->setEnabled(true);
    }
}

void SystemSet::slotBtnCal()
{
    //    QProcess *process = new QProcess;
    //    QString cmd = "wr chmod -x /etc/init.d/S85qt && wr chmod +x /etc/init.d/S85Cal && reboot";
    //    process->execute(cmd);
    ::system("wr chmod -x /etc/init.d/S85qt && wr chmod +x /etc/init.d/S85Cal && reboot");
}




void SystemSet::setSerialNum(QString serialNum)
{
    if(serialNum.isEmpty())
        return;
    ui->lineEdit_type->setText(serialNum.left(3));
    ui->lineEdit_date->setText(serialNum.mid(5,6));
    ui->lineEdit_times->setText(serialNum.right(3));
    ui->lineEdit_smallType->setText(serialNum.mid(3,2));
}

void SystemSet::editClear()
{
    ui->lineEdit_id->clear();
    ui->lineEdit_tem->clear();
    ui->lineEdit_net->clear();
    ui->lineEdit_have->clear();
    ui->lineEdit_alarm->clear();
}

void SystemSet::slotSuccess()
{
    QMessageBox::information(NULL,tr("设置提示"),tr("设置成功"),tr("关闭"));
}

void SystemSet::slotBtnUpdatePasswd()
{

    /*要根据用户登录的级别判断,执行那种权限的修改密码!
    *超级用户可以更高所有设置
    *管理员只可以更改操作员和自己的密码
    *操作员只能改自己的密码
    */
    int userType =  ui->comboBox_userType->currentIndex()+1;

    if(userType > m_userType)
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("权限不足！！！"),tr("关闭"));
        return;
    }


    QString oldPasswd = ui->lineEdit_oldPasswd->text();
    QString newPasswd = ui->lineEdit_newPasswd->text();
    if(newPasswd.isEmpty())
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("新密码不能为空！！！"),tr("关闭"));
        return;
    }

    if(oldPasswd == m_db.getUserPasswd(userType))
    {
        m_db.updateUserPasswd(userType,newPasswd);
        ui->lineEdit_oldPasswd->clear();ui->lineEdit_newPasswd->clear();
        QMessageBox::information(NULL,tr("操作提示"),tr("密码设置生效！！！"),tr("关闭"));
    }
    else
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("原始密码错误！！！"),tr("关闭"));
    }

}

void SystemSet::slotBtnClose()
{
    hide();
}

void SystemSet::slotBtnSerialNum()
{
    QString type  = ui->lineEdit_type->text();
    QString date  = ui->lineEdit_date->text();
    QString times = ui->lineEdit_times->text();
    QString smallType = ui->lineEdit_smallType->text();

    if(type.isEmpty() || date.isEmpty() || times.isEmpty() || smallType.isEmpty())
    {
        QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置失败！"),tr("确定"));
        return;
    }
    else
    {
        QString serialNum = type+smallType+date+times;
        //qDebug()<<"serialNum : "<<serialNum;
        bool flag = m_db.insertSerialNumber(serialNum);
        if(flag)
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置成功！\n重启生效!"),tr("确定"));
        }
        else
        {
            QMessageBox::information(NULL,tr("操作提示"),tr("主机序列号设置失败！"),tr("确定"));
        }
    }
}


