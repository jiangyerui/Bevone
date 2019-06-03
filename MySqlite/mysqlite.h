#ifndef MYSQLITE_H
#define MYSQLITE_H

#include <QtSql>
#include <QFile>
#include <QString>
#include <QMutex>
#include <QDebug>
#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQueryModel>

/*    主要功能
 * 1.初始化,链接数据库
 * 2.插入临时数据据
 * 3.获取主机序列号
 * 4.设置主机序列号
 * 5.获取序列号密钥
 * 6.获取密钥状态
 * 7.获取加锁状态
 * 8.获取节点地址
 * 9.获取打印配置
 *10.设置打印模式
 *11.获取用户密码
 *12.更新用户密码
 *13.获取轮询时间
 *14.设置轮询时间
 *15.获取总共时间
 *16.停止密钥时间
 *17.插入密钥启动总时间
 *18.获取数据行总数-1
 *19.获取数据行总数-2
 *20.获取数据行总数-3
 *21.删除数据
 *22.删除临时数据
 *23.恢复密码
 *24.设置未解锁
 *25.插入曲线数据
 *26.更新曲线数据
 *27.获取曲线参数值
 *28.
*/

class MySqlite : public QObject
{
    Q_OBJECT
public:
    explicit MySqlite(QObject *parent = 0);

    enum USERTYPE{
        USER  = 1,
        ADMIN = 2,
        SUPER = 3,
        QUIT  = 4

    };

    void initDb();//初始化,链接数据库

    void insertTemp(int net, int id, int type, uint time);//插入临时数据据
    void insertAlarm(int net, int id, int type, int status, QString value, uint time, QString address);//插入报警数据

    QString getSerialNumber();//获取主机序列号
    bool insertSerialNumber(QString serialNO);//设置主机序列号

    QString getEVN();//获取外部版本
    QString getIVN();//获取内部那本

    QString getSecretKey();//获取序列号密钥
    bool getSecretKeyFlag();//获取密钥状态
    bool getLockStatus();//获取加锁状态
    bool setSuccess(bool stutas);//设置未解锁
    QString getNodeAddress(int net,int id);//获取节点地址

    bool getPrintAlarm();
    void updateNode(QString net,QString id,QString enable);//更新节点，用于设置探测器个数jiang20190527
    void setPrintAlarm(bool model);

    bool getPrintError();
    void setPrintError(bool model);

    bool getPrintStyle();//获取打印配置
    void setPrintStyle(bool model);//设置打印模式

    QString getUserPasswd(int userType);//获取用户密码
    void updateUserPasswd(int userType, QString passwd);//更新用户密码

    uint getPollTime();//获取轮询时间
    bool setPollTime(QString time);//设置轮询时间

    qulonglong getPastTime();//获取已过时间
    void setPastTime(qulonglong time);//设置已过时间

    qulonglong getTotleTime();//获取总共时间
    bool stopSecretKeyTime();//停止密钥时间
    bool insertSecretKeyTime(ulong time, uint secretkeyflag);//插入密钥启动总时间

    uint getRowCount(QString &sql,QSqlQueryModel *model);//获取数据行总数-1
    uint getRowCount(QString &sql);//获取数据行总数-2
    uint getRowCount();//获取数据行总数-3

    bool delData(QString sql);//删除数据
    bool delTemp(int net, int id, int type);//删除临时数据
    void recoverPasswd();//恢复密码

    uint getNodeNum(uint net , uint id);
    bool setNodeNum(uint net , uint id);//jiang20190526
    //Curve
    bool insertCurveData(QString net, QString id, QString time, QString value);//插入曲线实时数据
    void updateCurveData(QString net, QString id, QString time, QString value);//更新数据
    QString getCurveData(QString net, QString id, QString starTime, QString endTime);//获取曲线参数值
    //SMS
    bool getSmsModel();
    bool setSmsModel(bool mode);
    void getSmsDetail(QString &centerNO,QString &receiverNO);
    bool setSmsDetail(QString centerNO,QString receiverNO);



private:
    QSqlDatabase m_db;

signals:

public slots:
};

#endif // MYSQLITE_H
