#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QScrollBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QTcpServer>
#include <QTcpSocket>
#include "MySqlite/mysqlite.h"
#include "SystemSet/systemset.h"
#include "Record/record.h"
#include "UserLogin/userlogin.h"
#include "MySqlite/mysqlite.h"
#include "watchdog/watchdog.h"
#include "GpioControl/gpiocontrol.h"
#include "SelfCheck/screencheck.h"
//#include "SendRS485/sendRS485.h"
#include "TcpServer/tcpserver.h"
#include "CalculaNode/calculanode.h"
//#include "SerialZigBee/serialhandle.h"
#include "CanMoudle/canmoudle.h"

#define NODENUM 1200
#define PAGEMAX 30

/*    主要功能
 * 1.初始化成员变量
 * 2.初始化信号槽
 * 3.初始化BTN节点
 * 4.初始化故障表格
 * 5.初始化报警表格
 * 6.故障查询下一条槽
 * 7.故障查询上一条槽
 * 8.报警查询下一条槽
 * 9.报警查询上一条槽
 *10.表格数据现实
 *11.节点按钮点击槽
 *12.节点查询跳首页槽
 *13.节点查询跳尾页槽
 *14.节点查询跳到页槽
 *15.登录界面显示槽
 *16.历史记录显示界面槽
 *17.设置界面显示槽
 *18.计算节点个数
 *19.节点显示与隐藏
 *20.页面节点上一页
 *21.页面节点下一页
 *22.声音开启-关闭
 *23.
 *24.
 *25.
 *26.
 *27.
*/

class SystemSet;
class UserLogin;
class Record;
class SelfCheck;
class CalculaNode;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer *m_currentTimer;
    QTimer *m_resetTimer;
    QTimer *m_selfCheckTimer;


    void initCan_1(const char *canName, int net, uint pollTime);
    void initCan_2(const char *canName, int net, uint pollTime);
    void initConfigure();//系统配置
    void initConnect();//初始化信号槽
    void initVar();//初始化成员变量
    void initNodeBtn();//初始化节点按钮
    void initTableWidget(QTableWidget *tableWidget);//初始化表格
    //显示数据表格
    void showData(QTableWidget *tableWidget, QSqlQueryModel*model, MySqlite *db, int type);
    void lcdNumberClean();//清空数据
    void setCurPageNum(int curPage);//设置当前页
//    void setCountPageNum(int countPage);//设置总数页
    void moduleStatus(int *node,int nodeNum,int curPage); //检测节点状态
    //int  calculationNode(int curNet);//计算网络节点个数
    //int  calculationPage(int regNum);//计算总总页数

    QString modType(int type);//探测器类型
    QString intToString(int number);//显示格式化
    void showNodeValue(int curNet, int curId);//探测器实时数据

    void hideAllTopWidget();


    //屏幕自检
    void selfCheckScreen();
    bool getPowerType(QString type);

    //SerialHandle *m_serialHandle;

    QSqlQueryModel *m_errorModel;
    QSqlQueryModel *m_alarmModel;
    QButtonGroup *m_btnGroup;

    MySqlite *m_db;
    SystemSet *sys ;
    Record *record;
    UserLogin *userlogin;
    ScreenCheck *m_screenCheck;


    CanMoudle *m_can2;
    CanMoudle *m_can1;
    uint m_delayNum;
    uint m_delayFlag;

    uint m_alarmNum;
    uint m_errorNum;

    GpioControl *m_gpio;

    //SendRS485 *m_rs485;

    CalculaNode * m_calNode;

    TcpServer *m_tcpServer;
    QThread *m_tcpThread;
    QThread *m_threadRS485;
private:

    uint m_freeTimes;

    uint m_lockTimes;
    bool m_kellLive;
    int m_curId;
    int m_curNet;//当前网络
    int m_curPage;//当前页
    int m_countPage;//页总数
    uint node[NODENUM];//显示节点数组
    int m_errorIndex;
    int m_alarmIndex;
    uint m_pollTime;
    bool m_powerType;//电源类型true=有备电;
    uchar m_modeSts;
    //自检屏幕颜色
    uint m_screenColor;
    //自己按开关
    bool m_selfCheckFlag;
    //声音开关
    bool m_soundFlag;
    //故障声音个数
    int m_etimes;
    //报警声音个数
    int m_atimes;
    //报警,故障重新打开
    uint m_reError;
    uint m_reAlarm;
    uint m_reDropped;
    uint m_reGPIO;

    uint m_nodeNum;

    QList<QTableWidgetItem *> m_itemAlarmList;
    QList<QTableWidgetItem *> m_itemErrorList;


    int m_userType;//用户类型


    QString m_styleSheet;
    QString m_redStyle;
    QString m_greenStyle;
    QString m_yellowStyle;
    QString m_blueStyle;

    QString m_normalStyle;
    QString m_pressedStyle;



signals:
    void sigUserLogin();       //登陆按钮信号
    void sigSetCurNet(int);
    void sigSetAlarmValue(int);//报警显示位置
    void sigSetErrorValue(int);//故障显示位置
    void sigLoginStatus(bool loginStatus);

private:
    Ui::MainWindow *ui;


public slots:

    void slotCurrentTime();
    void slotResetTimer();

    void slotBtnErrorUp();  //向上查询故障记录
    void slotBtnErrorDown();//向下查询故障记录
    void slotBtnAlarmUp();  //向上查询报警记录
    void slotBtnAlarmDown();//向下查询报警记录
    void slotBtnJump();  //跳转到指定页
    void slotBtnPass_1();//网络切换1
    void slotBtnPass_2();//网络切换2

    void slotBtnFirstPage();//首页
    void slotBtnTailPage(); //尾页
    void slotBtnLastPage(); //上一页
    void slotBtnNestPage(); //下一页
    void slotBtnClick(int index);//点击按钮
    void slotBtnSound();

    void slotSystemSetShow();//显示系统设置
    void slotRecordShow();   //显示历史记录
    void slotUserLoginShow();//用户登录
    void slotMainPageShow(); //密钥揭开后显示住界面
    void slotLogoutShow();   //登出
    void slotResetShow();    //复位
    void slotSelfCheckShow();//自检界面
    void slotDataShow(int type);//显示报警-故障数据
    void slotLoginStatus(int type);//登陆类型

    void selfCheckResult();


    void slotNodeData(int *node, int size, int countPage);

    void slotSound();

};

#endif // MAINWINDOW_H
