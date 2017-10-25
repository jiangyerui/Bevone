#ifndef RECORD_H
#define RECORD_H

#include <QFont>
#include <QList>
#include <QDebug>
#include <QWidget>
#include <QDateTimeEdit>
#include <QMessageBox>
#include <QTableWidget>

#include "MyPrint/myprint.h"
#include "MySqlite/mysqlite.h"


/*    主要功能
 * 1.历史记录查询
 * 2.数据删除单条
 * 3.数据删除整页
 * 4.数据删除全部
 * 5.数据打印单条
 * 6.数据打印整页
 * 7.数据打印全部(暂定无)
 * 8.上下翻页，跳转翻页
 * 9.曲线图显示历史数据
 *
 *
*/






namespace Ui {
class Record;
}

class Record : public QWidget
{
    Q_OBJECT

public:
    explicit Record(QWidget *parent = 0);
    ~Record();

    void initShow();
    QString setSelectSql(int item, bool net, bool id,QString &startTimer,QString &endTime);
    void connectPrint(QString net, QString id, QString type, QString status,QString value, QString time, QString address);
    void checkSelfPrint(QString nodeNum, QString alarmNum, QString errorNum, QString power, QString bpower);
    int m_userType;
private:
    Ui::Record *ui;

    QList<QTableWidgetItem *> m_itemList;
    QTableWidgetItem *m_item;
    QSqlQueryModel *m_model;

    MyPrint *m_myPrint;
    MySqlite m_db;
    int m_currentPage;
    int m_maxPage;
    QString temp;//定义一个临时变量

    int m_id;

    QFont m_font;


    void initVar();
    void initWidget();

    void initConnect();

    void delAllData();
    void delRowData(QString netStr, QString idStr, QString timeStr);
    void delPageData(int page);

    uint getPageCount(QString sql);
    void setBtnEnable(bool enabel,int current,int maxPage);
    void showData(QString sql, int currentPage);

    void setPrint(QTableWidget *tableWidget, MyPrint *myPrint, int row, int columnCount);


private slots:

    void slotSetNetEnable(bool enable);
    void slotSetIdEnable(bool enable);

    void slotBtnSearch();
    void slotBtnJump();
    void slotBtnDelData();
    void slotBtnUpPage();
    void slotBtnDownPage();
    void slotBtnPrint();

};

#endif // RECORD_H
