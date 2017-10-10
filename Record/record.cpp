#include "record.h"
#include "ui_record.h"

#include <QSqlQueryModel>

//#define DEBUG
#define ALLDATA    0
#define ALARMDATA  1
#define ERRORDATA  2
#define DROPPEDDATA  3

//record

#define ALARM  1
#define ERROR  2
#define CAN    3
#define POWER  4
#define BPOWER 5

Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(0,0,800,480);

    initVar();
    initWidget();
    initConnect();


    //只能是数字
    QRegExp regExpNumId("^([1-9][0-9]{1,2})|(101[0-9])|(102[0-4])|0$");//1-1024
    ui->lineEdit_address->setValidator(new QRegExpValidator(regExpNumId,this));
    QRegExp regExpNumNet("^[1-2]$");//1-2
    ui->lineEdit_pass->setValidator(new QRegExpValidator(regExpNumNet,this));


    QString styleStr = "QHeaderView::section{"
                       "background-color:rgb(255,255,255);"
                       "color: black;border: 1px solid #6c6c6c;}";
    ui->tableWdt_record->horizontalHeader()->setStyleSheet(styleStr);
    ui->tableWdt_record->verticalHeader()->setStyleSheet(styleStr);

    ui->tableWdt_record->horizontalHeader()->setEnabled(false);
    ui->tableWdt_record->horizontalHeader()->setVisible(true);//设置表头显示
    ui->tableWdt_record->horizontalHeader()->setStretchLastSection(true);
    ui->tableWdt_record->horizontalHeader()->setHighlightSections(false);

    //ui->tableWdt_record->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableWdt_record->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    ui->tableWdt_record->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    ui->tableWdt_record->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    QStringList headList;
    ui->tableWdt_record->setColumnCount(7);
    headList<<tr("通道")<<tr("地址")<<tr("类型")<<tr("状态")<<tr("数值")<<tr("时间")<<tr("详细地址");
    ui->tableWdt_record->setHorizontalHeaderLabels(headList);
    ui->tableWdt_record->horizontalHeader()->setFixedHeight(27);



    ui->tableWdt_record->setColumnWidth(0,50);
    ui->tableWdt_record->setColumnWidth(1,50);
    ui->tableWdt_record->setColumnWidth(2,60);
    ui->tableWdt_record->setColumnWidth(3,80);
    ui->tableWdt_record->setColumnWidth(4,80);
    ui->tableWdt_record->setColumnWidth(5,180);
    ui->tableWdt_record->setColumnWidth(6,180);

}

Record::~Record()
{
    delete ui;
}

QString Record::setSelectSql(int item, bool net, bool id, QString &startTimer, QString &endTime)
{
    QString netStr;
    QString idStr;
    QString sqlStr = "select net,id,type,status,value,time,address from RECORD where TIME >= "+startTimer+" and TIME <= "+endTime;

    if(item == ALLDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr;
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr;
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr;
        }
    }
    else if(item == ALARMDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and status = 1 ";
        }
        else if(net == false && id == false)//获取所有探测器报警信息
        {

            sqlStr += " and status = 1 ";
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and status = 1 ";
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and status = 1 ";
        }
    }
    else if(item == ERRORDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and status = 2 ";
        }
        else if(net == false && id == false)//获取所有探测器报警信息
        {

            sqlStr += " and status = 2 ";
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and status = 2 ";
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and status = 2 ";
        }
    }
    else if(item == DROPPEDDATA)
    {
        if(net == true && id == true)//获取指定的探测器故障信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and status = 3 ";
        }
        else if(net == false && id == false)//获取所有探测器故障信息
        {
            sqlStr += " and status == 3 ";
        }
        else if(net == true && id == false)//获取指定网络探测器故障信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and status = 3 ";
        }
        else if(net == false && id == true)//获取指定节点探测器故障信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and status = 3 ";
        }
    }

    return sqlStr;

}

void Record::initVar()
{
    m_font.setFamily(tr("文泉驿等宽微米黑"));
    m_font.setPointSize(9);
    m_id= 1;
    m_currentPage = 1;
    m_model = new QSqlQueryModel();

    m_myPrint = MyPrint::getPrint();
    m_myPrint->initCom();

}

void Record::initShow()
{
    this->show();
    m_currentPage = 1;

    ui->lb_count->setText("0");
    ui->lb_current->setText("0");
    ui->lineEdit_pageNum->clear();
    ui->tableWdt_record->clearContents();
    ui->tableWdt_record->setRowCount(0);
    ui->dTEdit_end->setDateTime(QDateTime::currentDateTime());
    ui->checkBox_net->setCheckState(Qt::Unchecked);
    ui->checkBox_id->setCheckState(Qt::Unchecked);
    ui->lineEdit_address->setEnabled(false);
    ui->lineEdit_pass->setEnabled(false);
    ui->pBtn_downPage->setEnabled(false);
    ui->pBtn_upPage->setEnabled(false);
    ui->pBtn_jump->setEnabled(false);
    ui->lineEdit_pageNum->setEnabled(false);

}

void Record::initWidget()
{

    //正则表达式表示只能输入大于0的正整数 只能是数字^[0-9]*$
    QRegExp regExp("^[1-9][0-9]*$");
    ui->lineEdit_pageNum->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit_address->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit_pass->setValidator(new QRegExpValidator(regExp, this));

    //上下翻页
    ui->pBtn_downPage->setEnabled(false);
    ui->pBtn_upPage->setEnabled(false);
    //net id 编辑框
    ui->lineEdit_address->setEnabled(false);
    ui->lineEdit_pass->setEnabled(false);
    //时间选择
    //ui->dTEdit_start->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    ui->dTEdit_end->setDateTime(QDateTime::currentDateTime());
    ui->dTEdit_start->setDateTime(QDateTime(QDate(2000,1,1),QTime(0,0,1)));


}


void Record::initConnect()
{
    connect(ui->pBtn_quit,SIGNAL(clicked(bool)),this,SLOT(hide()));
    connect(ui->pBtn_check,SIGNAL(clicked(bool)),this,SLOT(slotBtnSearch()));

    connect(ui->checkBox_id,SIGNAL(clicked(bool)),this,SLOT(slotSetIdEnable(bool)));
    connect(ui->checkBox_net,SIGNAL(clicked(bool)),this,SLOT(slotSetNetEnable(bool)));

    connect(ui->pBtn_downPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnDownPage()));
    connect(ui->pBtn_upPage,SIGNAL(clicked(bool)),this,SLOT(slotBtnUpPage()));
    connect(ui->pBtn_jump,SIGNAL(clicked(bool)),this,SLOT(slotBtnJump()));

    connect(ui->pBtn_del,SIGNAL(clicked(bool)),this,SLOT(slotBtnDelData()));
    connect(ui->pBtn_pagePrint,SIGNAL(clicked(bool)),this,SLOT(slotBtnPrint()));
}

void Record::delAllData()
{
    QString sql="delete from RECORD;";
    if(m_db.delData(sql))
    {
        ui->lb_count->setText("0");
        ui->lb_current->setText("0");
        ui->tableWdt_record->clearContents();
    }
    else
    {
        QMessageBox::warning(NULL,"删除数据","删除数据失败","确定");
    }
}

void Record::delRowData(QString netStr, QString idStr, QString timeStr)
{

    QString sql="delete from RECORD where net = "+netStr+" and id = "+idStr+" and time = "+timeStr+";";

    if(m_db.delData(sql))
    {
        if(ui->tableWdt_record->rowCount() == 1 && m_maxPage != 1)
        {
            m_currentPage--;
        }
    }
    else
    {
        QMessageBox::warning(NULL,tr("删除数据"),tr("删除数据失败"),tr("确定"));
    }
}

void Record::delPageData(int page)
{
    Q_UNUSED(page)
}


uint Record::getPageCount(QString sql)
{
    int countPage = 0;
    int recordNum = m_db.getRowCount(sql);//返回相应查询的条数
    if(0 >= recordNum)  //没有相应的记录返
    {
        return countPage;
    }
    else
    {
        if(recordNum % 10)//非整数页
        {
            countPage = recordNum/10;
            countPage++;
        }
        else//整数页
        {
            countPage = recordNum/10;
        }
    }

    return countPage;
}

void Record::setBtnEnable(bool enabel, int current, int maxPage)
{
    ui->pBtn_downPage->setEnabled(enabel);
    ui->pBtn_upPage->setEnabled(enabel);
    ui->pBtn_jump->setEnabled(enabel);
    ui->lineEdit_pageNum->setEnabled(enabel);
    ui->pBtn_pagePrint->setEnabled(enabel);
    ui->pBtn_del->setEnabled(enabel);
    ui->lb_count->setText(QString::number(maxPage));
    ui->lb_current->setText(QString::number(current));
}

#define MODULE_CUR  2 //漏电
#define MODULE_TEM  3 //温度

//显示数据
void Record::showData(QString sql,int currentPage)
{
    //设置每次返回的条数10条

    int baseNum = (currentPage - 1) * 10;
    sql += " ORDER BY time DESC limit 10 offset "+QString::number(baseNum)+";";

    int countNum = m_db.getRowCount(sql,m_model);//返回数据的总行数

#ifdef DEBUG
    qDebug()<<"sql      = "<<sql;
    qDebug()<<"countNum = "<<countNum;
#endif
    ui->tableWdt_record->clearContents();
    ui->tableWdt_record->setRowCount(countNum);//设置行数
    //qDeleteAll(m_itemList);
    m_itemList.clear();
    QTableWidgetItem *item;
    QSqlRecord record;

    for(int row = 0; row < countNum; row++)
    {
        record = m_model->record(row);

        ui->tableWdt_record->setRowHeight(row,34);
        for(int column = 0; column < 7; column++)
        {

            item = new QTableWidgetItem;
            m_itemList.append(item);
            m_font.setPointSize(15);
            item->setFont(m_font);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor(255,255,255));

            if(column == 0)
            {
                item->setText(record.value(column).toString());
                ui->tableWdt_record->setItem( row, column, item);
            }
            else if(column == 1)
            {
                item->setText(record.value(column).toString());
                ui->tableWdt_record->setItem( row, column, item);
            }
            else if(column == 2)
            {

                if(MODULE_CUR == record.value(column))
                {
                    item->setText(tr("漏电"));
                }
                else if(MODULE_TEM == record.value(column))
                {
                    item->setText(tr("测温"));
                }
                ui->tableWdt_record->setItem( row, column, item);
            }

            else if(column == 3)
            {
                if(POWER == record.value(column))
                {
                    item->setText(tr("主电故障"));
                    ui->tableWdt_record->setItem( row, column, item);
                }
                else if(BPOWER == record.value(column))
                {
                    item->setText(tr("备电故障"));
                    ui->tableWdt_record->setItem( row, column, item);
                }
                else if(ALARM == record.value(column))
                {
                    item->setText(tr("模块报警"));
                    ui->tableWdt_record->setItem( row, column, item);
                }
                else if(ERROR == record.value(column))
                {
                    item->setText(tr("模块故障"));
                    ui->tableWdt_record->setItem( row, column, item);
                }
                else if(CAN == record.value(column))
                {
                    item->setText(tr("通讯故障"));
                    ui->tableWdt_record->setItem( row, column, item);
                }
            }
            else if(column == 4)
            {
                item->setText(record.value(column).toString());
                ui->tableWdt_record->setItem( row, column, item);
            }
            else if(column == 5)
            {
                int dt = record.value(column).toString().toUInt();
                QDateTime datetime;
                datetime.setTime_t(dt);
                QString dtstr=datetime.toString("yyyy-MM-dd/hh:mm:ss");
                item->setText(dtstr);
                ui->tableWdt_record->setItem( row, column, item);
            }
            else if(column == 6)
            {
                item->setText(record.value(column).toString());
                ui->tableWdt_record->setItem( row, column, item);
            }

        }
    }
}

void Record::connectPrint(QString net, QString id, QString type, QString status,QString value,QString time, QString address)
{
    m_myPrint->printConnect(net, id,type,status,value,time,address);
}

void Record::slotSetNetEnable(bool enable)
{
    ui->lineEdit_pass->setEnabled(enable);
}
void Record::slotSetIdEnable(bool enable)
{
    ui->lineEdit_address->setEnabled(enable);
}

void Record::slotBtnSearch()
{

    int item = ui->cbBox_type->currentIndex();
    bool net = ui->checkBox_net->isChecked();
    bool  id = ui->checkBox_id->isChecked();



    uint starTime = ui->dTEdit_start->dateTime().toTime_t();
    uint endTime  = ui->dTEdit_end->dateTime().toTime_t();

    if(starTime == endTime)
    {
        QMessageBox::question( NULL, tr("提示信息"), tr("请选择时间范围!"), tr("确定"));
    }
    else if(starTime > endTime)
    {
        QMessageBox::question( NULL, tr("提示信息"), tr("选择时间范围错误!"), tr("确定"));
    }
    else
    {
        QString countSql;
        QString starTimeStr = QString::number(starTime);
        QString endTimeStr  = QString::number(endTime);
        QString sqlStr = setSelectSql(item,net,id,starTimeStr,endTimeStr);

        QString str ="WHERE";
        countSql = "select count(*) from RECORD WHERE" + sqlStr.mid(sqlStr.indexOf(str,0,Qt::CaseInsensitive)+5,sqlStr.size() - sqlStr.indexOf(str,0))+";";
        m_maxPage = getPageCount(countSql);

#ifdef  DEBUG
        qDebug()<<"net           = "<<net;
        qDebug()<<"id            = "<<id;
        qDebug()<<"countSql      = "<<countSql;
        qDebug()<<"sqlStr        = "<<sqlStr;
        qDebug()<<"m_maxPage     = "<<m_maxPage;
        qDebug()<<"m_currentPage = "<<m_currentPage;
#endif

        if(m_currentPage > m_maxPage )
        {
            m_currentPage = 1;
        }

        if(m_maxPage > 0)
        {

            showData(sqlStr,m_currentPage);
            setBtnEnable(true,m_currentPage,m_maxPage);
        }
        else if(m_maxPage == 0)
        {
            ui->tableWdt_record->clearContents();
            ui->tableWdt_record->setRowCount(0);//设置行数
            setBtnEnable(false,0,0);
            ui->pBtn_pagePrint->setEnabled(true);
        }
    }
}

void Record::slotBtnJump()
{
    int num = ui->lineEdit_pageNum->text().toInt();
    if(num > m_maxPage)
    {
        QMessageBox::question( NULL, tr("提示信息"), tr("不能超过最页数!"), tr("确定"));
        ui->lineEdit_pageNum->setText(QString::number(m_maxPage));
    }
    else
    {
        m_currentPage = num;
        slotBtnSearch();
    }
}

void Record::slotBtnDelData()
{
    int index = ui->QCom_del->currentIndex();
    if(index == 0)
    {
        int row = ui->tableWdt_record->currentRow();
        if(row < 0)
        {
            return;
        }
        //先删除数据
        QString netStr  = ui->tableWdt_record->item(row,0)->text();
        QString idStr   = ui->tableWdt_record->item(row,1)->text();
        QString timeStr = ui->tableWdt_record->item(row,5)->text();
        uint second     = QDateTime::fromString(timeStr,"yyyy-MM-dd/hh:mm:ss").toTime_t();
        delRowData(netStr,idStr,QString::number(second));
        //查询数据
        ui->tableWdt_record->clearContents();
        slotBtnSearch();
    }
    else if(index == 1)
    {
        int rowCount = ui->tableWdt_record->rowCount();
        QString netStr,idStr,timeStr;
        uint second ;
        for(int row = rowCount-1;0 <= row; row--)
        {
            netStr  = ui->tableWdt_record->item(row,0)->text();
            idStr   = ui->tableWdt_record->item(row,1)->text();
            timeStr = ui->tableWdt_record->item(row,5)->text();
            second  = QDateTime::fromString(timeStr,"yyyy-MM-dd/hh:mm:ss").toTime_t();
            delRowData(netStr,idStr,QString::number(second));
        }
        ui->tableWdt_record->clearContents();
        if(m_currentPage == m_maxPage && m_maxPage != 1)
        {
            m_currentPage--;
        }
        slotBtnSearch();
    }
    else if(index == 2)
    {
        delAllData();
        slotBtnSearch();
    }
}

void Record::slotBtnUpPage()
{
    if(m_currentPage > 1)
    {
        m_currentPage--;
        slotBtnSearch();
        ui->pBtn_upPage->setEnabled(true);
    }
    else if(m_currentPage == 1 || m_maxPage == 0)
    {
        ui->pBtn_upPage->setEnabled(false);
        QMessageBox::information(NULL,tr("提示"),tr("已经是第一页！"),tr("确定"));
    }
    ui->pBtn_downPage->setEnabled(true);
}

void Record::slotBtnDownPage()
{
    if(m_currentPage < m_maxPage)
    {
        m_currentPage++;
        slotBtnSearch();
        ui->pBtn_downPage->setEnabled(true);
    }
    else if(m_currentPage == m_maxPage || m_maxPage == 0)
    {
        ui->pBtn_downPage->setEnabled(false);
        QMessageBox::information(NULL,tr("提示"),tr("已经是最后一页！"),tr("确定"));
    }
    ui->pBtn_upPage->setEnabled(true);
}

void Record::setPrint(QTableWidget *tableWidget, MyPrint *myPrint, int row, int columnCount)
{
    QList <QString> printList;
    for(int column = 0;column < columnCount;column++)
    {
        QString str = tableWidget->item(row,column)->text();
        printList.insert(column,str);
    }

    QString net    = printList.at(0);
    QString id     = printList.at(1);
    QString type   = printList.at(2);
    QString status = printList.at(3);
    QString value  = printList.at(4);
    QString time   = printList.at(5);
    QString add    = printList.at(6);

    myPrint->printConnect(net, id, type,status,value, time,add);
    printList.clear();
}

void Record::slotBtnPrint()
{

    int index = ui->QCom_del->currentIndex();
    if(index == 0)
    {
        //判断有没有选中一行
        int row = ui->tableWdt_record->currentRow();
        if(row >= 0)
        {
            setPrint(ui->tableWdt_record,m_myPrint,row,7);
        }
    }
    else if(index == 1)
    {
        int rowNum = ui->tableWdt_record->rowCount();
        if(rowNum > 0)
        {
            for(int row = 0;row < rowNum;row++)
            {
                setPrint(ui->tableWdt_record,m_myPrint,row,7);
            }
        }
    }
}


