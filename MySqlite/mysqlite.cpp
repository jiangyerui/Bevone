#include "mysqlite.h"
#include <QDebug>
#include <QCryptographicHash>

MySqlite::MySqlite(QObject *parent) : QObject(parent)
{

}

void MySqlite::initDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE"); //添数据库驱
    m_db.setDatabaseName("./Bevone.db");
    if (m_db.open())
    {
        qDebug("**** MySqlite open success ****") ;
    }
    else
    {
        qDebug("**** MySqlite open error ****");
    }

}

void MySqlite::insertTemp(int net, int id, int type,uint time)
{
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString typeStr = QString::number(type);
    QString timeStr = QString::number(time);
    QString sql = "insert into TEMP values("+netStr+","+idStr+","+typeStr+","+timeStr+");";
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

void MySqlite::insertAlarm(int net,int id,int type,int status,QString value,uint time,QString address)
{
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString typeStr = QString::number(type);
    QString timeStr = QString::number(time);
    QString staStr  = QString::number(status);
    QString sql = "insert into RECORD values("+netStr+","+idStr+","+typeStr+","+staStr+","+value+","+timeStr+","+"'"+address+"');";
    qDebug()<<"sql : "<<sql;
    m_db.transaction();
    QSqlQuery query(m_db);
    query.exec(sql);
    m_db.commit();
    query.clear();
}


bool MySqlite::insertSerialNumber(QString serialNO)
{

    bool flag = false;
    if(NULL == serialNO)
        return flag;
    QByteArray bytePwd = serialNO.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    QString strPwdMd5 = (bytePwdMd5.toHex().left(12)).toUpper();
    //qDebug()<<"strPwdMd5 = "<<strPwdMd5;
    QString sql = "update serialnum set serialnumber = '"+serialNO+"',secretkey = '"+strPwdMd5+"' where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;

}

QString MySqlite::getEVN()
{
    QString getEVN;
    QString sql = "select EVN from VERSION where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            getEVN  = query.value(0).toString();
        }
    }
    query.clear();
    return getEVN;
}

QString MySqlite::getIVN()
{
    QString getIVN;
    QString sql = "select IVN from VERSION where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            getIVN  = query.value(0).toString();
        }
    }
    query.clear();
    return getIVN;
}

QString MySqlite::getSerialNumber()
{
    QString serialNum;
    QString sql = "select serialnumber from serialnum where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            serialNum  = query.value(0).toString();
        }
    }
    query.clear();
    return serialNum;
}

QString MySqlite::getSecretKey()
{
    QString secretkey;
    QString sql = "select secretkey from serialnum where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            secretkey  = query.value(0).toString();
        }
    }
    query.clear();
    return secretkey;
}

bool MySqlite::getSecretKeyFlag()
{
    bool flag = false;
    QString sql = "select secretkeyflag from SYSTEM where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            flag  = query.value(0).toBool();
        }
    }
    query.clear();
    return flag;
}

bool MySqlite::getLockStatus()
{
    bool flag = false;
    QString sql = "select success from SYSTEM where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            flag  = query.value(0).toBool();
        }
    }
    query.clear();
    return flag;
}

QString MySqlite::getNodeAddress(int net, int id)
{
    QString addStr;
    QString netStr = QString::number(net);
    QString idStr = QString::number(id);
    QString sql = "select address from NODE where net = "+netStr+" and id ="+idStr+";";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            addStr = query.value(0).toString();
        }
    }
    query.clear();
    return addStr;

}

bool MySqlite::getPrintError()
{
    bool printFlag = false;
    QString sql = "select ERROR from PRINT where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            printFlag  = query.value(0).toBool();
        }
    }
    query.clear();
    return printFlag;
}

void MySqlite::setPrintError(bool model)
{
    QString sql;
    sql.clear();
    if(model == true)
    {
        sql = "update PRINT set ERROR = 1 where rowid = 1;";
    }
    else
    {
        sql = "update PRINT set ERROR = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

void MySqlite::setPrintAlarm(bool model)
{
    QString sql;
    sql.clear();
    if(model == true)
    {
        sql = "update PRINT set ALARM = 1 where rowid = 1;";
    }
    else
    {
        sql = "update PRINT set ALARM = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

bool MySqlite::getPrintAlarm()
{
    bool printFlag = false;
    QString sql = "select ALARM from PRINT where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            printFlag  = query.value(0).toBool();
        }
    }
    query.clear();
    return printFlag;
}

void MySqlite::setPrintStyle(bool model)
{
    QString sql;
    sql.clear();
    if(model == true)
    {
        sql = "update PRINT set MODE = 1 where rowid = 1;";
    }
    else
    {
        sql = "update PRINT set MODE = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

bool MySqlite::getPrintStyle()
{
    bool printFlag = false;
    QString sql = "select MODE from PRINT where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            printFlag  = query.value(0).toBool();
        }
    }
    query.clear();
    return printFlag;
}

void MySqlite::updateUserPasswd(int userType, QString passwd)
{
    QString sql;
    sql.clear();
    sql = "update PASSWD set passwd = "+passwd+" where usertype = "+QString::number(userType)+";";
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

QString MySqlite::getUserPasswd(int userType)
{
    QString passwd;
    QString sql = "select passwd from PASSWD where usertype = "+QString::number(userType)+";";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            passwd = query.value(0).toString();
            //            qDebug()<<"passwd = "<<passwd;
        }
    }
    query.clear();
    return passwd;
}

bool MySqlite::insertSecretKeyTime(ulong time,uint secretkeyflag = 0)
{
    QString sql;
    bool flag = false;
    sql.clear();
    sql = "update SYSTEM set totletime = "+QString::number(time)+",secretkeyflag = "
            +QString::number(secretkeyflag)+" where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

bool MySqlite::stopSecretKeyTime()
{
    QString sql;
    bool flag = false;
    sql.clear();
    sql = "update SYSTEM set totletime = 0,pasttime = 0, secretkeyflag = 0 where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

bool MySqlite::setPollTime(QString time)
{
    bool flag = false;
    QString sql;
    sql.clear();
    sql = "update SYSTEM set polltime = "+time+" where rowid = 1;";
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

uint MySqlite::getPollTime()
{
    uint pollTime = 0;
    QString sql = "select polltime from SYSTEM where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            pollTime  = query.value(0).toULongLong();
        }
    }
    query.clear();
    return pollTime;
}

void MySqlite::setPastTime(qulonglong time)
{
    QString sql;
    sql.clear();
    sql = "update SYSTEM set pasttime = "+QString::number(time)+",secretkeyflag = 1 where rowid = 1;";
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();

}

qulonglong MySqlite::getPastTime()
{
    qulonglong pastTime = 0;
    QString sql = "select pasttime from SYSTEM where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            pastTime  = query.value(0).toULongLong();
        }
    }
    query.clear();
    return pastTime;
}

qulonglong MySqlite::getTotleTime()
{
    qulonglong totelTime = 0;
    QString sql = "select totletime from SYSTEM where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            totelTime  = query.value(0).toULongLong();
        }
    }
    query.clear();
    return totelTime;
}

uint MySqlite::getRowCount(QString &sql, QSqlQueryModel *model)
{
    int r=0;
    model->setQuery(sql,m_db);//链接数据库，设定查询语句
    if (model->rowCount()<0)//返回数据库的行数
    {
        qDebug()<<"model error !";
    }
    else
    {
        r = model->rowCount();
    }

    return r;
}

uint MySqlite::getRowCount(QString &sql)
{
    int count = 0;
    QSqlQuery query(m_db);

    bool flag = query.exec(sql);

    if(flag)
    {
        if(query.next())
        {
            count = query.value(0).toUInt();
            //qDebug()<<"count = "<<count;
        }
    }
    else
    {
        //qDebug()<<"MySqlite.cpp 277 error \n";
    }
    query.finish();
    query.clear();

    return count;
}

uint MySqlite::getRowCount()
{
    int count = 0;
    QString sql = "select count(*) from RECORD;";
    QSqlQuery query(m_db);

    bool flag = query.exec(sql);

    if(flag)
    {
        if(query.next())
        {
            count = query.value(0).toUInt();
        }

    }
    else
    {
        qDebug()<<"MySqlite.cpp 208 error \n";
    }
    query.finish();
    query.clear();

    return count;
}

bool MySqlite::delData(QString sql)
{
    bool flag;

    QSqlQuery query(m_db);
    m_db.transaction();
    if(query.exec(sql))
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    m_db.commit();
    query.finish();
    query.clear();
    return flag;
}

bool MySqlite::delTemp(int net, int id,int type)
{
    bool flag;
    QString netStr  = QString::number(net);
    QString idStr   = QString::number(id);
    QString typeStr = QString::number(type);
    QString sql = "delete from TEMP where net = "+netStr+" and id = "+idStr+" and type = "+typeStr+";";
    QSqlQuery query(m_db);
    m_db.transaction();
    if(query.exec(sql))
    {
        flag = true;
    }
    else
    {
        flag = false;
    }
    m_db.commit();
    query.finish();
    query.clear();
    return flag;
}

void MySqlite::recoverPasswd()
{
    updateUserPasswd(MySqlite::USER,  QString("111111"));
    updateUserPasswd(MySqlite::ADMIN, QString("222222"));
    updateUserPasswd(MySqlite::SUPER, QString("333333"));
}

uint MySqlite::getNodeNum(uint net, uint id)
{
    uint exsit = 0;

    QString sql = "select count(*) from NODE WHERE net = "+QString::number(net)+" and id = "+QString::number(id)+";";
    qDebug()<<"getNodeNum slq"<<sql;
    QSqlQuery query(m_db);

    if(query.exec(sql))
    {
        if(query.next())
        {
            exsit = query.value(0).toUInt();
        }
    }
    query.clear();
    return exsit;
}

bool MySqlite::setSuccess(bool stutas)
{
    bool flag = false;
    QString sql;
    sql.clear();
    if(stutas == TRUE)
    {
        sql = "update SYSTEM set success = 1 where rowid = 1;";
    }
    else
    {
        sql = "update SYSTEM set success = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

bool MySqlite::insertCurveData(QString net, QString id, QString time, QString value)
{
    bool flag = false;
    QString sql = "insert into CURVE values("+net+","+id+","+value+","+time+");";
    qDebug()<<sql;
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        flag = true;
    }
    query.clear();
    return flag;
}

void MySqlite::updateCurveData(QString net, QString id, QString time, QString value)
{
    QString sql = "update CURVE set value = "+value+",time = "+time+" where net = "+net+" and id = "+id+";";
    //qDebug()<<"sql = "<<sql;
    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();
}

QString MySqlite::getCurveData(QString net, QString id, QString starTime,QString endTime)
{
    QString time;
    QString value;
    QString results;
    QDateTime datetime;

    QString sql = "select value,time from Curve where "
                  "net = "+net+" and id = "+id+" and TIME >= "+starTime+" and TIME <="+endTime+";";

    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        while(query.next())
        {
            value = query.value(0).toString()+"#";
            int dt= query.value(1).toUInt();
            datetime.setTime_t(dt);
            time  = datetime.toString("yyyy-MM-dd hh:mm:ss")+"#";
            results += (time + value);
        }
    }
    query.clear();
    return results;
}

bool MySqlite::getSmsModel()
{
    bool SmsModel = false;
    QString sql = "select MODEL from SMS where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            SmsModel  = query.value(0).toBool();
        }
    }
    query.clear();
    return SmsModel;
}

bool MySqlite::setSmsModel(bool mode)
{
    bool flag = false;
    QString sql;sql.clear();
    if(mode == TRUE)
    {
        sql = "update SMS set MODEL = 1 where rowid = 1;";
    }
    else
    {
        sql = "update SMS set MODEL = 0 where rowid = 1;";
    }
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

void MySqlite::getSmsDetail(QString &centerNO, QString &receiverNO)
{
    QString sql = "select CENTER, RECIEVER from SMS where rowid = 1;";
    QSqlQuery query(m_db);
    if(query.exec(sql))
    {
        if(query.next())
        {
            centerNO  = query.value(0).toString();
            receiverNO  = query.value(1).toString();
        }
    }
    query.clear();
}

bool MySqlite::setSmsDetail(QString centerNO, QString receiverNO)
{
    bool flag = false;
    QString sql;
    sql.clear();
    qDebug();
    sql = "update SMS set CENTER = "+centerNO+",RECEIVER = "+receiverNO+" where rowid = 1;";
    qDebug()<<sql;
    QSqlQuery query(m_db);
    flag = query.exec(sql);
    query.clear();
    return flag;
}

