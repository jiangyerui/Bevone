#include "mainwindow.h"
#include "ui_mainwindow.h"


#define ALADATA     1
#define ERODATA     2
#define HOURTIME    60*1000
#define SECENDTIME  1000
#define TIMEOUT     10
#define GPIODEV     "/dev/gpio"
#define DEVICE      "/dev/watchdog"
//#define DEBUG
//#define Memery
//#define WatchDog  //看门狗
#define TCP_IP    //网络传输
//#define RS485     //RS485


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);
    initVar();
    initConfigure();
    initConnect();
    initNodeBtn();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initCan_1(const char *canName,int net,uint pollTime)
{
    QThread *can0Thread = new QThread;
    m_can1 = new CanMoudle(canName,net,pollTime);
    m_can1->moveToThread(can0Thread);
    can0Thread->start();
}

void MainWindow::initCan_2(const char *canName,int net,uint pollTime)
{
    QThread *can1Thread = new QThread;
    m_can2 = new CanMoudle(canName,net,pollTime);
    m_can2->moveToThread(can1Thread);
    can1Thread->start();
}

void MainWindow::initConfigure()
{

    ui->pBtn_Set->setEnabled(false);
    ui->pBtn_Reset->setEnabled(false);
    ui->pBtn_Check->setEnabled(false);
}

void MainWindow::initConnect()
{
    m_currentTimer = new QTimer;
    connect(m_currentTimer,SIGNAL(timeout()),this,SLOT(slotCurrentTime()));//系统检测定时器
    m_currentTimer->start(SECENDTIME);

    m_resetTimer = new QTimer;
    connect(m_resetTimer,SIGNAL(timeout()),this,SLOT(slotResetTimer()));
    m_resetTimer->start(300);

    connect(m_can1,SIGNAL(sigSuccess()),sys,SLOT(slotSuccess()));
    connect(m_can2,SIGNAL(sigSuccess()),sys,SLOT(slotSuccess()));
    qDebug()<<"***********************";
    qDebug()<<"m_can1 : "<<m_can1;
    qDebug()<<"m_can2 : "<<m_can2;
    //网络操作信号槽
    connect(ui->pBtn_Record,SIGNAL(clicked(bool)),this,SLOT(slotRecordShow()));
    connect(ui->pBtn_User,SIGNAL(clicked(bool)),this,SLOT(slotUserLoginShow()));
    connect(ui->pBtn_Set,SIGNAL(clicked(bool)),this,SLOT(slotSystemSetShow()));
    connect(ui->pBtn_Reset,SIGNAL(clicked(bool)),this,SLOT(slotResetShow()));
    connect(ui->pBtn_Check,SIGNAL(clicked(bool)),this,SLOT(slotSelfCheckShow()));
    connect(ui->pBtn_Voice,SIGNAL(clicked(bool)),this,SLOT(slotBtnSound()));

    connect(m_btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));

    connect(ui->pBtn_jump,SIGNAL(clicked(bool)),this,SLOT(slotBtnJump()));
    connect(ui->pBtn_pass1,SIGNAL(clicked(bool)),this,SLOT(slotBtnPass_1()));
    connect(ui->pBtn_pass2,SIGNAL(clicked(bool)),this,SLOT(slotBtnPass_2()));
    connect(ui->pBtn_lastPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnLastPage()));
    connect(ui->pBtn_nestPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnNestPage()));
    connect(ui->pBtn_tailPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnTailPage()));
    connect(ui->pBtn_firstPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnFirstPage()));

    connect(ui->pBtn_downErrorStrip,SIGNAL(clicked(bool)),this,SLOT(slotBtnErrorDown()));
    connect(ui->pBtn_upErrorStrip,SIGNAL(clicked(bool)),this,SLOT(slotBtnErrorUp()));
    connect(this,SIGNAL(sigSetErrorValue(int)),ui->tabWid_Error->verticalScrollBar(),SLOT(setValue(int)));

    connect(ui->pBtn_downAlarmStrip,SIGNAL(clicked(bool)),this,SLOT(slotBtnAlarmDown()));
    connect(ui->pBtn_upAlarmStrip,SIGNAL(clicked(bool)),this,SLOT(slotBtnAlarmUp()));
    connect(this,SIGNAL(sigSetAlarmValue(int)),ui->tabWid_Alarm->verticalScrollBar(),SLOT(setValue(int)));
    /*************/
    connect(m_calNode,SIGNAL(sigBtnSound()),this,SLOT(slotSound()));
    connect(m_calNode,SIGNAL(sigNode(int*,int)),this,SLOT(slotNodeData(int*,int)));
    connect(m_calNode,SIGNAL(sigSetCountPageNum(int)),this,SLOT(setCountPageNum(int)));

}

void MainWindow::initVar()
{

    m_freeTimes = 1;

    m_curPage = 1;
    m_userType = 1;
    m_lockTimes = 1;
    m_reGPIO = 0;
    m_reError = 0;
    m_reAlarm = 0;
    m_reDropped = 0;
    m_curNet = 1;

    m_etimes = 0;
    m_atimes = 0;

    m_screenColor = 0;
    m_delayFlag = false;
    m_delayNum = 0;
    //自检开关
    m_selfCheckFlag = false;
    m_kellLive = true;
    m_soundFlag = true;
    m_errorIndex = 0;
    m_alarmIndex = 0;
    m_styleSheet   = "border:1px solid;font: 14pt '文泉驿等宽微米黑';border-radius:10px;"
                     "border-color: rgb(53, 156, 212);color: rgb(0, 0, 0);";
    m_redStyle     = m_styleSheet+"background-color: rgb(255, 0,   0);";
    m_greenStyle   = m_styleSheet+"background-color: rgb(0, 255,   0);";
    m_yellowStyle  = m_styleSheet+"background-color: rgb(255, 255, 0);";
    m_blueStyle    = m_styleSheet+"background-color: rgb(82, 143, 237);";

    m_normalStyle  = "border:1px solid;border-radius:8px;"
                     "color: rgb(255, 255, 255);border-color:rgb(53, 156, 212);";
    m_pressedStyle = "border:1px solid;border-radius:8px;"
                     "color: rgb(0 ,0, 0);background-color:rgb(85, 170, 255);";
    //初始化故障,报警显示
    initTableWidget(ui->tabWid_Alarm);
    initTableWidget(ui->tabWid_Error);

    ui->lb_address->clear();
    ui->lb_curNodeNum->clear();
    ui->pBtn_pass1->setStyleSheet(m_pressedStyle);
    ui->pBtn_pass2->setStyleSheet(m_normalStyle);
    //设置跳转页面只能输入大于0<value<26
    QRegExp regExp("^([1-9]|[12][0-6])$");
    //QRegExp regExp("^([1-9])|(1[0-9])|(2[0-6])$");
    ui->lineEdit->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit->clear();


    m_db         = new MySqlite;
    m_btnGroup   = new QButtonGroup;
    m_alarmModel = new QSqlQueryModel();
    m_errorModel = new QSqlQueryModel();

#ifdef TCP_IP
    //网络接口
    m_tcpServer = new TcpServer;
    m_tcpThread = new QThread;
    m_tcpServer->moveToThread(m_tcpThread);
    m_tcpThread->start();
#endif

#ifdef  RS485
    //485通讯第三方
    m_threadRS485 = new QThread;
    m_rs485 = new SendRS485;
    int ret = m_rs485->initRS485("/dev/ttymxc2");
    m_rs485->moveToThread(m_threadRS485);
    if(ret == 1)
    {
        m_threadRS485->start();
    }
#endif

#ifdef WatchDog
    //看门狗 参数:char* 设备名称 ,int 溢出时间
    Watchdog::init((char*)DEVICE,TIMEOUT);
#endif

    sys = new SystemSet;
    record = new Record;
    //屏幕检测观察坏点
    m_screenCheck = new ScreenCheck;
    m_screenCheck->hide();
    m_gpio = new GpioControl;
    //初始化全局变量
    GlobalData::initData();
    //根据序列号确定网络和节点个数
    GlobalData::initCmdType(m_db->getSerialNumber());
    //电源类型
    m_powerType = getPowerType(m_db->getSerialNumber());
    //计算节点状态线程
    QThread *thread = new QThread;
    m_calNode    = new CalculaNode();
    m_calNode->moveToThread(thread);
    m_calNode->initVar(m_powerType);
    thread->start();

    //ZigBee通讯
    //    QThread *serialThread = new QThread;
    //    m_serialHandle = new SerialHandle();
    //    m_serialHandle->initSerial(1);
    //    m_serialHandle->moveToThread(serialThread);
    //    serialThread->start();

    //获取设置的轮循时间间隔
    m_pollTime = m_db->getPollTime();

#ifdef DEBUG
    qDebug()<<"SerialNumber = "<<m_db->getSerialNumber();
    qDebug()<<"m_powerType  = "<<m_powerType;
    qDebug()<<"m_pollTime   = "<<m_pollTime;
#endif
    //初始化网络1,网络2不可见
    slotBtnPass_1();
    if(netMax == 2)
    {
        initCan_1("can0",1,m_pollTime);
        ui->pBtn_pass2->setVisible(false);
    }
    else
    {
        initCan_1("can0",1,m_pollTime);
        initCan_2("can1",2,m_pollTime);
    }


}

void MainWindow::showData(QTableWidget *tableWidget,QSqlQueryModel *model,MySqlite *db, int type)
{
    QString sql ;
    int countNum = 0;
    if(type == ERODATA)
    {
        //error
        sql ="select net,id,time FROM TEMP WHERE type >= 2 and type <= 5 ORDER BY time DESC;";
        countNum = db->getRowCount(sql,model);//返回数据的总行数
        m_errorNum = countNum;
        QString titleNum = tr("故障: ")+QString::number(countNum)+tr("个");
        ui->groupBox_Error->setTitle(titleNum);

        m_itemErrorList.clear();
        qDeleteAll(m_itemErrorList);
    }
    else
    {
        //alarm
        sql ="select net,id,time FROM TEMP WHERE type = 1 ORDER BY time DESC;";
        countNum = db->getRowCount(sql,model);//返回数据的总行数
        m_alarmNum = countNum;
        QString titleNum = tr("报警: ")+QString::number(countNum)+tr("个");
        ui->groupBox_Alarm->setTitle(titleNum);

        m_itemAlarmList.clear();
        qDeleteAll(m_itemAlarmList);
    }

    tableWidget->clearContents();
    tableWidget->setRowCount(countNum);//设置行数
    tableWidget->setColumnWidth(0,60);
    tableWidget->setColumnWidth(1,160);



    QTableWidgetItem *item;
    QSqlRecord record;

    QFont font = QApplication::font();
    font.setPointSize(14);


    for(int row = 0; row < countNum; row++)
    {
        record = model->record(row);
        tableWidget->setRowHeight(row,15);
        for(int column = 0; column < 2; column++)
        {

            item = new QTableWidgetItem;
            if(type == ERODATA)
            {
                m_itemErrorList.append(item);
            }
            else
            {
                m_itemAlarmList.append(item);
            }
            item->setFont(font);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor(255,255,255));

            if(column == 0)
            {
                QString netStr = record.value(0).toString();
                item->setText(netStr+"-"+intToString(record.value(1).toInt()));
                tableWidget->setItem(row, column, item);
            }
            else if(column == 1)
            {
                uint dt = record.value(2).toString().toUInt();
                QDateTime datetime;
                datetime.setTime_t(dt);
                QString dtstr=datetime.toString("yyyy-MM-dd/hh:mm:ss");
                item->setText(dtstr);
                tableWidget->setItem(row, column, item);
            }
        }
    }
}

void MainWindow::lcdNumberClean()
{
    //温度
    ui->lcdNo_alaTem->display("0");//温度报警
    ui->lcdNo_curTemp->display("0"); //温度实时
    ui->lcdNo_alaTemSet->display("0");//温度锁定
    //漏电
    ui->lcdNo_Leakage->display("0");//固有漏电
    ui->lcdNo_curLeakage->display("0");//实时漏电
    ui->lcdNo_alaLeakage->display("0");//漏电报警
    ui->lcdNo_alaLeakageSet->display("0");//漏电设定
}

void MainWindow::setCountPageNum(int countPage)
{
    QString countpageStr = QString::number(countPage);
    ui->lb_countPage->setText(countpageStr);
}

void MainWindow::slotSound()
{
    m_soundFlag = TRUE;
    ui->lb_Voice->setText(tr("蜂  鸣"));
    ui->pBtn_Voice->setIcon(QIcon(":/Image/sound.png"));
}

void MainWindow::setCurPageNum(int curPage)
{
    QString curPageStr = QString::number(curPage);
    ui->lb_curPage->setText(curPageStr);

}
//计算节点个数
int MainWindow::calculationNode(int curNet)
{
    int regNum = 0;
    for(int i=0;i<IDNUM;i++)
    {
        node[i] = 0;
    }

    for(int net = 0;net < NETNUM;net++)
    {
        for(int id = 0;id < IDNUM;id++)
        {
            if(mod[net][id].used == true)
            {
                if(curNet == net)
                {
                    node[regNum++] = id;
                    node[regNum]   = 0;
                }
            }
        }
    }
    ui->lb_nodeNum->setText(QString::number(regNum));
    return regNum;
}

int MainWindow::calculationPage(int regNum)
{
    int countPage = 0;
    if(regNum < 40)
    {
        countPage = 1;
    }
    else
    {
        for(int i = 1;i < PAGEMAX;i++)
        {
            if((regNum > 40*i)&&( regNum <= 40*(i+1)))
            {
                countPage = i+1;
            }
        }
    }
    return countPage;
}
//节点显示与隐藏
void MainWindow::moduleStatus(int curPage)
{

    m_modeSts = 0;
    int i = (curPage - 1 ) * 40;

    for(int index = 1;index <= 40;index++)
    {
        if(mod[m_curNet][node[i]].used == true && node[i] != 0 && node[i] <= idMax && i < IDNUM)
        {
            int type = mod[m_curNet][node[i]].type;
            QString text = QString::number(node[i]) + modType(type);
            m_btnGroup->button(index)->setText(text);
            m_btnGroup->button(index)->setVisible(true);
            if(mod[m_curNet][node[i]].normalFlag == TRUE)
            {
                m_modeSts = 0;
                m_btnGroup->button(index)->setStyleSheet(m_greenStyle);
            }
            if(mod[m_curNet][node[i]].errorFlag == TRUE)
            {
                m_modeSts = 2;
                m_btnGroup->button(index)->setStyleSheet(m_yellowStyle);
            }
            if(mod[m_curNet][node[i]].dropFlag == TRUE)
            {
                m_modeSts = 9;
                m_btnGroup->button(index)->setStyleSheet(m_yellowStyle);
            }
            if(mod[m_curNet][node[i]].alarmFlag == TRUE)
            {
                m_modeSts = 1;
                m_btnGroup->button(index)->setStyleSheet(m_redStyle);
            }
#ifdef TCP_IP
//            //网络传输数据
//            if(type == MODULE_CUR)
//            {
//                uint data = mod[m_curNet][node[i]].rtData;
//                m_tcpServer->addNetData(type,m_curNet,node[i],m_modeSts,data,0);
//            }
//            else if(type == MODULE_TEM)
//            {
//                uint temp = mod[m_curNet][node[i]].temData;
//                m_tcpServer->addNetData(type,m_curNet,node[i],m_modeSts,0,temp);
//            }
#endif
        }
        else
        {
            m_btnGroup->button(index)->setText("");
            m_btnGroup->button(index)->setVisible(false);
        }
        i++;
    }
}

QString MainWindow::modType(int type)
{
    QString typeStr;
    switch (type) {
    case MODULE_CUR:
        typeStr = tr("\n漏电");
        break;
    case MODULE_TEM:
        typeStr = tr("\n温度");
        break;
    case MODULE_ELE:
        typeStr = tr("\n电弧");
        break;
    case MODULE_PYR:
        typeStr = tr("\n热解");
        break;
    case MODULE_LIM:
        typeStr = tr("\n限流");
        break;
    case MODULE_LIN:
        typeStr = tr("\n感温");
        break;
    default:
        break;
    }
    return typeStr;
}

void MainWindow::showNodeValue(int curNet, int curId)
{
    lcdNumberClean();
//    if(mod[curNet][curId].errorFlag == TRUE || mod[curNet][curId].dropFlag == TRUE)
//    {
//        return;
//    }

    int alarmTemSet  = mod[curNet][curId].alarmTemSet;
    int temData      = mod[curNet][curId].temData;
    int alarmTem     = mod[curNet][curId].alarmTem;


    int rtData       = mod[curNet][curId].rtData;      //实时报警值
    int baseData     = mod[curNet][curId].baseData;    //固有报警值
    int alarmDataSet = mod[curNet][curId].alarmDataSet;//报警设定值
    int alarmData    = mod[curNet][curId].alarmData;   //报警值


    //温度
    ui->lcdNo_curTemp->display(QString::number(temData));
    ui->lcdNo_alaTem->display(QString::number(alarmTem));
    ui->lcdNo_alaTemSet->display(QString::number(alarmTemSet));
    //漏电
    ui->lcdNo_Leakage->display(QString::number(baseData));
    ui->lcdNo_curLeakage->display(QString::number(rtData));
    ui->lcdNo_alaLeakage->display(QString::number(alarmData));
    ui->lcdNo_alaLeakageSet->display(QString::number(alarmDataSet));

}

void MainWindow::hideAllTopWidget()
{
    foreach(QWidget *hubwidget,QApplication::topLevelWidgets())
    {
        if((hubwidget->objectName()!="MainWindow"))
        {
            if(hubwidget->isVisible())
            {
                hubwidget->hide();
            }
        }
    }
}



void MainWindow::selfCheckScreen()
{
    if(m_screenColor <= 5)
    {
        m_screenColor++;
        m_screenCheck->setColor(m_screenColor);
    }

    if(m_screenColor == 6)
    {
        m_screenColor = 0;
        m_screenCheck->hide();
        m_selfCheckFlag = false;
        m_calNode->setSelfCheckFlag(m_selfCheckFlag);

        int nodeNum = calculationNode(1) + calculationNode(2);

        QString nodeNumStr = QString::number(nodeNum);
        QString alarmNumStr = QString::number(m_alarmNum);
        QString errorNumStr = QString::number(m_errorNum);
        QString power,bpower;
        if(g_powerStatus == true)   power = tr("正常");
        else    power = tr("故障");

        if(g_bpowerStatus == true)  bpower = tr("正常");
        else    bpower = tr("故障");

        QString result = tr(" 节点总数:")+nodeNumStr+
                         tr(" 报警总数:")+alarmNumStr+
                         tr(" 故障总数:")+errorNumStr+
                         tr(" 主电状态:")+power+
                         tr(" 备电状态:")+bpower;
        QMessageBox::information(NULL,tr("自检结果"),result,tr("确定"));

        record->checkSelfPrint(nodeNumStr,alarmNumStr,errorNumStr,power,bpower);

        qDebug()<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
    }
}

bool MainWindow::getPowerType(QString type)
{
    bool flag = false;
    if(type.isEmpty())
        return flag;
    if(type.mid(2,1) == "N")
    {
        flag = true;
    }
    return flag;
}
//核心操作
void MainWindow::slotCurrentTime()
{
    //显示当前时间
    QDateTime dateTime=QDateTime::currentDateTime();
    QString dtstr=dateTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->lb_Time->setText(dtstr);

    //显示内存情况
#ifdef Memery
    m_freeTimes++;
    if(m_freeTimes == 6)
    {
        m_freeTimes = 1;
        ::system("free");
    }
#endif

    if(g_login)
    {
        hideAllTopWidget();//隐藏所有界面
        slotLoginStatus(MySqlite::QUIT);//实现退出登录
        emit sigLoginStatus(false);//设置退出值
    }


    //显示当前前节点的数值
    showNodeValue(m_curNet,m_curId);
    //显示故障临时数据
    slotDataShow(ALADATA);
    //显示报警临时数据
    slotDataShow(ERODATA);

    //自检操作是其他指示灯,蜂鸣器不受控制
    if(m_selfCheckFlag == true)
    {
        selfCheckScreen();//屏幕自检
    }

#ifdef WatchDog
    //看门口狗
    Watchdog::kellLive();
#endif

    if(m_delayFlag == true)
    {
        m_delayNum++;
        if(m_delayNum == 15)
        {
            m_delayNum = 0;
            if(netMax == 2)
            {
                m_can1->controlTimer(true);
            }
            else
            {
                m_can1->controlTimer(true);
                m_can2->controlTimer(true);
            }
            m_delayFlag = false;
            g_resetCmd = false;
        }
    }

}
//检测按键复位状态
void MainWindow::slotResetTimer()
{
    if(m_gpio->dealReset() == true && m_userType != 1)
    {
        slotResetShow();
    }
}
//故障向上翻页
void MainWindow::slotBtnErrorUp()
{
    if(m_errorIndex <= 0)
    {
        m_errorIndex = 0;
    }
    else
    {
        m_errorIndex--;
    }
    emit sigSetErrorValue(m_errorIndex);
}
//故障向下翻页
void MainWindow::slotBtnErrorDown()
{
    m_errorIndex++;
    QString sql;
    sql = "select count(*) from TEMP WHERE  type >= 2 and type <= 5;";
    int count = m_db->getRowCount(sql);
    if(count == m_errorIndex)
    {
        m_errorIndex = count - 1;
    }
    emit sigSetErrorValue(m_errorIndex);
}
//报警向上翻页
void MainWindow::slotBtnAlarmUp()
{
    if(m_alarmIndex <= 0)
    {
        m_alarmIndex = 0;
    }
    else
    {
        m_alarmIndex--;
    }
    emit sigSetAlarmValue(m_alarmIndex);

}
//报警向下翻页
void MainWindow::slotBtnAlarmDown()
{
    m_alarmIndex++;
    QString sql;
    sql = "select count(*) FROM TEMP WHERE type = 1;";

    int count = m_db->getRowCount(sql);
    if(count == m_alarmIndex)
    {
        m_alarmIndex = count - 1;
    }
    emit sigSetAlarmValue(m_alarmIndex);
}

void MainWindow::slotBtnJump()
{
    if(ui->lineEdit->text().isEmpty() == TRUE)
    {
        QMessageBox::information(this,tr("提示"),tr("请输入有效数字！"),tr("确定"));
        return;
    }
    else
    {
        int page = ui->lineEdit->text().toInt();
        m_countPage = calculationPage(calculationNode(m_curNet));
        if(page > m_countPage && page != 0)
        {
            ui->lineEdit->clear();
            QMessageBox::information(this,tr("提示"),tr("页数超过最大总数，请重新输入！"),tr("确定"));
            return;
        }
        else
        {
            m_curPage = page;
            moduleStatus(m_curPage);
            setCurPageNum(m_curPage);
        }
    }
}

void MainWindow::slotBtnPass_1()
{
    m_curNet = 1;
    m_calNode->setCurNet(m_curNet);
    ui->lb_curNodeNum->clear();
    ui->lb_address->clear();
    ui->pBtn_pass1->setStyleSheet(m_pressedStyle);
    ui->pBtn_pass2->setStyleSheet(m_normalStyle);
}

void MainWindow::slotBtnPass_2()
{
    m_curNet = 2;
    m_calNode->setCurNet(m_curNet);
    ui->lb_curNodeNum->clear();
    ui->lb_address->clear();
    ui->pBtn_pass1->setStyleSheet(m_normalStyle);
    ui->pBtn_pass2->setStyleSheet(m_pressedStyle);
}

void MainWindow::slotBtnFirstPage()
{
    m_countPage = calculationPage(calculationNode(m_curNet));
    if(m_countPage != 0)
    {
        m_curPage = 1;
        moduleStatus(m_curPage);
        setCurPageNum(m_curPage);
    }
}

void MainWindow::slotBtnTailPage()
{
    m_countPage = calculationPage(calculationNode(m_curNet));
    if(m_countPage != 0)
    {
        m_curPage = m_countPage;
        moduleStatus(m_curPage);
        setCurPageNum(m_curPage);
    }
}

void MainWindow::slotBtnLastPage()
{
    m_countPage = calculationPage(calculationNode(m_curNet));
    if(m_curPage <= 1)
    {
        QMessageBox::information(this,tr("提示"),tr("已经是第一页！"),tr("确定"));
        return;
    }
    else
    {
        m_curPage--;
        moduleStatus(m_curPage);
        setCurPageNum(m_curPage);
    }
}

void MainWindow::slotBtnNestPage()
{

    m_countPage = calculationPage(calculationNode(m_curNet));
    if(m_curPage == m_countPage)
    {
        QMessageBox::information(this,tr("提示"),tr("已经是最后页！"),tr("确定"));
        return;
    }
    else
    {
        m_curPage++;
        moduleStatus(m_curPage);
        setCurPageNum(m_curPage);
    }
}

void MainWindow::slotBtnClick(int index)
{
    QString currentNodeStr = m_btnGroup->button(index)->text();
    currentNodeStr = currentNodeStr.left(currentNodeStr.length()-3);
    //当前节点
    ui->lb_curNodeNum->setText(currentNodeStr);
    m_curId = currentNodeStr.toInt();

    ui->lb_address->clear();
    QString nodeAdd = m_db->getNodeAddress(m_curNet,m_curId);
    ui->lb_address->setText(nodeAdd);
}

void MainWindow::slotBtnSound()
{
    if(m_soundFlag == TRUE)
    {
        m_soundFlag = FALSE;
        ui->lb_Voice->setText(tr("静  音"));
        ui->pBtn_Voice->setIcon(QIcon(":/Image/mute.png"));
    }
    else
    {
        m_soundFlag = TRUE;
        ui->lb_Voice->setText(tr("蜂  鸣"));
        ui->pBtn_Voice->setIcon(QIcon(":/Image/sound.png"));
    }
    m_calNode->setSound(m_soundFlag);
}

void MainWindow::slotSystemSetShow()
{
    sys->systemShow();
}

void MainWindow::slotRecordShow()
{
    record->initShow();
}

void MainWindow::slotUserLoginShow()
{
    emit sigUserLogin();
}

void MainWindow::slotMainPageShow()
{
    this->show();
}

void MainWindow::slotLogoutShow()
{
    QMessageBox::question(NULL,tr("节点注销"),tr("可以选择(单个注销)与(全部注销)操作！"),tr("单个注销"),tr("全部注销"),tr("取消"));
}

void MainWindow::slotResetShow()
{
    int ret = QMessageBox::question(NULL,tr("复位操作"),tr("可以复位所有节点状态！"),tr("全部当前"),tr("取消"));
    if(ret == 0)
    {
        if(m_delayFlag == false )
        {
            if(netMax == 2)
            {
                g_resetCmd = true;
                m_can1->controlTimer(false);
            }
            else
            {
                g_resetCmd = true;
                m_can1->controlTimer(false);
                m_can2->controlTimer(false);
            }
        }


        QString styleSheet =m_styleSheet+"background-color: rgb(0, 255, 0);font: 14pt";
        for(int i = 1;i<= 40;i++)
        {
            if(m_btnGroup->button(i)->isVisible())
            {
                m_btnGroup->button(i)->setStyleSheet(styleSheet);
            }
        }

        for(int net = 0;net < NETNUM;net++)
        {
            for(int id = 0;id < IDNUM;id++)
            {

                mod[net][id].id = 0;
                mod[net][id].net = 0;
                mod[net][id].alarmTem = 0;
                mod[net][id].temData  = 0;
                mod[net][id].rtData   = 0;
                mod[net][id].baseData = 0;
                mod[net][id].alarmData= 0;
                mod[net][id].alarmFlag = FALSE;
                mod[net][id].dropFlag  = FALSE;

                mod[net][id].normalFlag = TRUE;
                mod[net][id].errorFlag  = FALSE;
                mod[net][id].insertAlarm = FALSE;
                mod[net][id].insertDrop   = FALSE;
                mod[net][id].insertError  = FALSE;
                mod[net][id].insertNormal = FALSE;
                mod[net][id].alaTemLock   = FALSE;
                mod[net][id].alaDataLock  = FALSE;
            }
        }
        //
        m_db->delData("delete from TEMP;");//删除临时记录
        ui->lb_curNodeNum->clear();
        //slotBtnTailPage();
        m_delayFlag = true;

    }
    else
    {
        return;
    }
}

void MainWindow::slotSelfCheckShow()
{
    m_selfCheckFlag = true;
    m_screenCheck->show();
    m_gpio->selfCheck();
    m_calNode->setSelfCheckFlag(m_selfCheckFlag);
}

void MainWindow::slotDataShow(int type)
{
    if(type == ERODATA)
    {
        showData(ui->tabWid_Error,m_errorModel,m_db,type);
    }
    else if(type == ALADATA)
    {
        showData(ui->tabWid_Alarm,m_alarmModel,m_db,type);
    }
}

void MainWindow::slotLoginStatus(int type)
{

    if(type == MySqlite::USER)
    {
        ui->lb_User->setText(tr("操作员"));
        ui->pBtn_Set->setEnabled(true);
        ui->pBtn_Reset->setEnabled(false);
        ui->pBtn_Check->setEnabled(false);
        sys->m_userType = type;
        m_userType = type;
    }
    else if(type == MySqlite::ADMIN)
    {
        ui->lb_User->setText(tr("管理员"));
        ui->pBtn_Set->setEnabled(true);
        ui->pBtn_Reset->setEnabled(true);
        ui->pBtn_Check->setEnabled(true);
        sys->m_userType = type;
        m_userType = type;
    }
    else if(type == MySqlite::SUPER)
    {
        ui->lb_User->setText(tr("超级用户"));
        m_userType = type;
        sys->m_userType = type;
        ui->pBtn_Set->setEnabled(true);
        ui->pBtn_Reset->setEnabled(true);
        ui->pBtn_Check->setEnabled(true);
    }
    else if(type == MySqlite::QUIT)
    {
        m_userType = 1;
        ui->lb_User->setText(tr("用  户"));
        ui->pBtn_Set->setEnabled(false);
        ui->pBtn_Reset->setEnabled(false);
        ui->pBtn_Reset->setEnabled(false);
        ui->pBtn_Check->setEnabled(false);
        g_login = false;
    }
}

void MainWindow::initNodeBtn()
{
    m_btnGroup->addButton(ui->pBtn_1, 1);
    m_btnGroup->addButton(ui->pBtn_2, 2);
    m_btnGroup->addButton(ui->pBtn_3, 3);
    m_btnGroup->addButton(ui->pBtn_4, 4);
    m_btnGroup->addButton(ui->pBtn_5, 5);
    m_btnGroup->addButton(ui->pBtn_6, 6);
    m_btnGroup->addButton(ui->pBtn_7, 7);
    m_btnGroup->addButton(ui->pBtn_8, 8);
    m_btnGroup->addButton(ui->pBtn_9, 9);
    m_btnGroup->addButton(ui->pBtn_10,10);
    m_btnGroup->addButton(ui->pBtn_11,11);
    m_btnGroup->addButton(ui->pBtn_12,12);
    m_btnGroup->addButton(ui->pBtn_13,13);
    m_btnGroup->addButton(ui->pBtn_14,14);
    m_btnGroup->addButton(ui->pBtn_15,15);
    m_btnGroup->addButton(ui->pBtn_16,16);
    m_btnGroup->addButton(ui->pBtn_17,17);
    m_btnGroup->addButton(ui->pBtn_18,18);
    m_btnGroup->addButton(ui->pBtn_19,19);
    m_btnGroup->addButton(ui->pBtn_20,20);
    m_btnGroup->addButton(ui->pBtn_21,21);
    m_btnGroup->addButton(ui->pBtn_22,22);
    m_btnGroup->addButton(ui->pBtn_23,23);
    m_btnGroup->addButton(ui->pBtn_24,24);
    m_btnGroup->addButton(ui->pBtn_25,25);
    m_btnGroup->addButton(ui->pBtn_26,26);
    m_btnGroup->addButton(ui->pBtn_27,27);
    m_btnGroup->addButton(ui->pBtn_28,28);
    m_btnGroup->addButton(ui->pBtn_29,29);
    m_btnGroup->addButton(ui->pBtn_30,30);
    m_btnGroup->addButton(ui->pBtn_31,31);
    m_btnGroup->addButton(ui->pBtn_32,32);
    m_btnGroup->addButton(ui->pBtn_33,33);
    m_btnGroup->addButton(ui->pBtn_34,34);
    m_btnGroup->addButton(ui->pBtn_35,35);
    m_btnGroup->addButton(ui->pBtn_36,36);
    m_btnGroup->addButton(ui->pBtn_37,37);
    m_btnGroup->addButton(ui->pBtn_38,38);
    m_btnGroup->addButton(ui->pBtn_39,39);
    m_btnGroup->addButton(ui->pBtn_40,40);

    QString styleSheet =m_styleSheet+"background-color: rgb(0, 255, 0);font: 14pt";
    for(int i = 1;i<= 40;i++)
    {
        m_btnGroup->button(i)->setFocusPolicy(Qt::NoFocus);
        m_btnGroup->button(i)->setVisible(false);
        m_btnGroup->button(i)->setStyleSheet(styleSheet);
    }
}

void MainWindow::initTableWidget(QTableWidget *tableWidget)
{
    QStringList alrmHeadList;
    tableWidget->setColumnCount(2);
    alrmHeadList<<tr("节   点")<<tr("时   间");
    tableWidget->setHorizontalHeaderLabels(alrmHeadList);
    tableWidget->horizontalHeader()->setFixedHeight(20);
    tableWidget->setColumnWidth(0,70);
    tableWidget->setColumnWidth(1,170);

    QString horStyle = "QHeaderView::section{"
                       "background-color:rgb(255,255,255);"
                       "color: black;border: 1px solid #6c6c6c;}";;
    QString verStyle = "QHeaderView::section {"
                       "background-color:rgb(255,255,255);"
                       "color: black;border: 1px solid #6c6c6c; padding-left: 4px;}";
    tableWidget->horizontalHeader()->setStyleSheet(horStyle);
    tableWidget->verticalHeader()->setStyleSheet(verStyle);
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);
    tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionMode (QAbstractItemView::NoSelection); //设置选择模式，选择单行
}

QString MainWindow::intToString(int number)
{
    QString str ;
    if(0<number && number < 10)
    {
        str = "000"+QString::number(number) ;
    }
    else if(10 <= number && number < 100)
    {
        str = "00"+QString::number(number) ;
    }
    else if(100 <= number && number < 1000)
    {
        str = "0"+QString::number(number) ;
    }
    else
    {
        str = QString::number(number);
    }

    return str;
}

void MainWindow::slotChangeNodeColor(int id, int net)
{

    int i = (m_curPage-1) *  40;

    if((node[i] != 0) && (i < IDNUM) && (m_curNet == QString::number(net).toInt()))
    {
        for(int index = 1; index <= 40;index++)
        {
            if(mod[m_curNet][node[i]].id == id)
            {
                int type = mod[m_curNet][node[i]].type;
                QString text = QString::number(node[i]) + modType(type);
                m_btnGroup->button(index)->setText(text);
                m_btnGroup->button(index)->setVisible(true);

                //int flag = mod[m_curNet][node[i]].flag;

                if(mod[m_curNet][node[i]].normalFlag == TRUE)
                {
                    m_btnGroup->button(index)->setStyleSheet(m_greenStyle);
                }
                if(mod[m_curNet][node[i]].errorFlag == TRUE)
                {
                    m_btnGroup->button(index)->setStyleSheet(m_yellowStyle);
                }
                if(mod[m_curNet][node[i]].dropFlag == TRUE)
                {
                    m_btnGroup->button(index)->setStyleSheet(m_yellowStyle);
                }
                if(mod[m_curNet][node[i]].alarmFlag == TRUE)
                {
                    m_btnGroup->button(index)->setStyleSheet(m_redStyle);
                }

                moduleStatus(m_curPage);
            }
            i++;
        }
    }
}

void MainWindow::slotNodeData(int *node, int size)
{
    for(int i = 0;i<size;i++)
    {
        this->node[i] = node[i];
    }

    ui->lb_nodeNum->setText(QString::number(size));
    moduleStatus(m_curPage);//当前页面的显示和隐藏

}





