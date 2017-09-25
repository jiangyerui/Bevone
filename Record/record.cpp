#include "record.h"
#include "ui_record.h"

#include <QSqlQueryModel>

#define DEBUG
#define ALLDATA    0
#define ALARMDATA  1
#define ERRORDATA  2
#define DROPPEDDATA  2

//record
#define POWER  4
#define BPOWER 5
#define ALARM  1
#define ERROR  2
#define CAN    3

Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(0,0,800,480);

    initVar();
    initCurve();
    initWidget();
    initConnect();



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
    ui->tableWdt_record->setColumnCount(5);
    headList<<tr("通道")<<tr("地址")<<tr("类型")<<tr("时间")<<tr("详细地址");
    ui->tableWdt_record->setHorizontalHeaderLabels(headList);
    ui->tableWdt_record->horizontalHeader()->setFixedHeight(27);


    ui->tableWdt_record->setColumnWidth(0,50);
    ui->tableWdt_record->setColumnWidth(1,50);
    ui->tableWdt_record->setColumnWidth(2,100);
    ui->tableWdt_record->setColumnWidth(3,200);
    ui->tableWdt_record->setColumnWidth(4,150);

}

Record::~Record()
{
    delete ui;
}

QString Record::setSelectSql(int item, bool net, bool id, QString &startTimer, QString &endTime)
{
    QString netStr;
    QString idStr;
    QString sqlStr = "select net,id,type,time,address from RECORD where TIME >= "+startTimer+" and TIME <="+endTime;

    if(item == ALLDATA)
    {
        return sqlStr;
    }
    else if(item == ALARMDATA)
    {
        if(net == true && id == true)//获取指定的探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and type = 1 ";
        }
        else if(net == false && id == false)//获取所有探测器报警信息
        {

            sqlStr += " and type = 1 ";
        }
        else if(net == true && id == false)//获取指定网络探测器报警信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and type = 1 ";
        }
        else if(net == false && id == true)//获取指定节点探测器报警信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and type = 1 ";
        }
    }
    else if(item == DROPPEDDATA)
    {
        if(net == true && id == true)//获取指定的探测器故障信息
        {
            netStr = ui->lineEdit_pass->text();
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and net = "+ netStr +" and id = "+idStr+" and type = 3 ";
        }
        else if(net == false && id == false)//获取所有探测器故障信息
        {

            sqlStr += " and type == 3 ";
        }
        else if(net == true && id == false)//获取指定网络探测器故障信息
        {
            netStr = ui->lineEdit_pass->text();
            sqlStr += " and net = "+ netStr +" and type = 3 ";
        }
        else if(net == false && id == true)//获取指定节点探测器故障信息
        {
            idStr  = ui->lineEdit_address->text();
            sqlStr += " and id = "+idStr+" and type = 3 ";
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
    //ui->dTEditCurve_end->setDateTime(QDateTime::currentDateTime());
    ui->lineEdit_address->setEnabled(false);
    ui->lineEdit_pass->setEnabled(false);
    ui->pBtn_downPage->setEnabled(false);
    ui->pBtn_upPage->setEnabled(false);
    ui->pBtn_jump->setEnabled(false);
    ui->lineEdit_pageNum->setEnabled(false);
    //ui->tabWidget->setCurrentWidget(ui->tab_Record);
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
    ui->dTEdit_start->setDateTime(QDateTime(QDate(1970,1,1),QTime(0,0,1)));

    //ui->tabWidget->hide();
}

void Record::initCurve()
{

//    ui->widget->resize(775, 375);
//    QString results = "2016-06-01 08:00:00#36#"
//                      "2016-06-01 12:00:00#37#"
//                      "2016-06-01 18:00:00#37#"

//                      "2016-06-02 08:00:00#36#"
//                      "2016-06-02 12:00:00#37#"
//                      "2016-06-02 18:00:00#37#"

//                      "2016-06-03 08:00:00#36#"
//                      "2016-06-03 12:00:00#35#"
//                      "2016-06-03 18:00:00#37#"

//                      "2016-06-04 08:00:00#36#"
//                      "2016-06-04 12:00:00#37#"
//                      "2016-06-04 18:00:00#37#"

//                      "2016-06-05 08:00:00#36#"
//                      "2016-06-05 12:00:00#37#"
//                      "2016-06-05 18:00:00#36#"

//                      "2016-06-06 08:00:00#37#"
//                      "2016-06-06 12:00:00#36#"
//                      "2016-06-06 18:00:00#37#"

//                      "2016-06-07 08:00:00#37#"
//                      "2016-06-07 12:00:00#37#"
//                      "2016-06-07 18:00:00#36#";


//    ui->widget->setLocale(QLocale(QLocale::Chinese, QLocale::China));
//    ui->widget->addGraph();
//    QPen pen;
//    pen.setColor(QColor(0, 0, 255, 200));
//    ui->widget->graph(0)->setLineStyle(QCPGraph::lsLine);//设置数据点由一条直线连接
//    ui->widget->graph(0)->setPen(pen);

//    ui->widget->addGraph(); // 红色点
//    ui->widget->graph(1)->setPen(QPen(Qt::red));
//    ui->widget->graph(1)->setLineStyle(QCPGraph::lsNone);
//    ui->widget->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
//    // 显示图例
//    ui->widget->graph(0)->setName(tr("漏电值"));
//    ui->widget->graph(1)->setName(tr("具体数值"));
//    ui->widget->legend->setVisible(true);

//    //GraphDemo(results);//调用我们写的函并且将数据传给这个函数

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

    //connect(ui->pBtnCurve_check,SIGNAL(clicked(bool)),this,SLOT(slotpBtnCurveCheck()));
    //connect(ui->pBtnCurve_quit,SIGNAL(clicked(bool)),this,SLOT(slotpBtnCurveQuit()));
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

void Record::GraphDemo(QString results)
{
    Q_UNUSED(results)
//    QStringList list = results.split("#");//假设数据
//    int listSize = list.size();
//    int max = listSize / 2;
//    QVector<double> time(max), value(max);//给数组赋值，时间和数值 7*3=21个数据

//    for(int i=0; i < max; ++i)
//    {
//        time[i] = QDateTime::fromString(list[2*i],"yyyy-MM-dd hh:mm:ss").toTime_t();
//        temp = list.at(2*i+1);
//        value[i] = temp.toInt();
//    }

//    ui->widget->graph(0)->setData(time, value);//设置数据
//    ui->widget->graph(0)->rescaleValueAxis();
//    ui->widget->graph(1)->addData(time, value);//设置点

//    //配置下轴显示日期和时间，而不是数字：
//    ui->widget->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//    ui->widget->xAxis->setDateTimeFormat("yyyy-MM-dd\nhh:mm:ss");
//    // 设置一个更紧凑的字体大小为底部和左轴刻度标签：
//    ui->widget->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ui->widget->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    // 设置一天为固定的一个刻度
//    ui->widget->xAxis->setAutoTickStep(false);
//    ui->widget->xAxis->setTickStep(3600); // 一天的秒数
//    //    ui->widget->xAxis->setTickStep(3600*24); // 一天的秒数
//    ui->widget->xAxis->setSubTickCount(9);//一个大刻度包含4个小刻度
//    // 设置轴标签
//    ui->widget->xAxis->setLabel(tr("时间(h)"));
//    ui->widget->yAxis->setLabel(tr("漏电(mA)"));
//    // 设置上边和右边轴没有刻度和标签
//    ui->widget->xAxis2->setVisible(true);
//    ui->widget->yAxis2->setVisible(true);//坐标轴是否可见
//    ui->widget->xAxis2->setTicks(false);//刻度是否可见
//    ui->widget->yAxis2->setTicks(false);
//    ui->widget->xAxis2->setTickLabels(false);//轴标签是否可见
//    ui->widget->yAxis2->setTickLabels(false);

//    // 设置轴范围和显示全部数据
//    //    ui->widget->xAxis->setRange(time[0],time[0]+24*3600*7);
//    ui->widget->xAxis->setRange(time[0],time[max-1]);
//    ui->widget->yAxis->setRange(0, 1000);

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
        for(int column = 0; column < 5; column++)
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
            else if(column == 3)
            {
                int dt = record.value(column).toString().toUInt();
                QDateTime datetime;
                datetime.setTime_t(dt);
                QString dtstr=datetime.toString("yyyy-MM-dd/hh:mm:ss");
                item->setText(dtstr);
                ui->tableWdt_record->setItem( row, column, item);
            }
            else if(column == 4)
            {
                item->setText(record.value(column).toString());
                ui->tableWdt_record->setItem( row, column, item);
            }

        }
    }
}

void Record::connectPrint(QString net, QString id, int type, QString time, QString address)
{
    QString typeStr;typeStr.clear();
    if(ALARM == type)
    {
        typeStr = tr("火灾报警");
    }
    else if(ERROR == type)
    {
        typeStr = tr("模块故障");
    }
    else if(CAN == type)
    {
        typeStr = tr("通讯故障");
    }
    m_myPrint->printConnect(net, id,typeStr,time,address);
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

        if(DROPPEDDATA == item)
        {
            countSql = "select count(*) from RECORD WHERE type = 3;";
        }
        else if(ERROR == item)
        {
            countSql = "select count(*) from RECORD WHERE type = 2;";
        }
        else if(ALARMDATA == item)
        {
            countSql = "select count(*) from RECORD WHERE type = 1;";
        }
        else if(ALLDATA == item)
        {
            countSql = "select count(*) from RECORD;";
        }

        m_maxPage = getPageCount(countSql);

#ifdef  DEBUG
        qDebug()<<"net = "<<net;
        qDebug()<<"id  = "<<id;
        qDebug()<<"sqlStr = "<<sqlStr;
        qDebug()<<"m_maxPage     = "<<m_maxPage;
        qDebug()<<"m_currentPage = "<<m_currentPage;
#endif

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
        QString timeStr = ui->tableWdt_record->item(row,3)->text();
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
            timeStr = ui->tableWdt_record->item(row,3)->text();
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
    //qDebug()<<"printList = "<<printList;

    QString net  = printList.at(0);
    QString id   = printList.at(1);
    QString type = printList.at(2);
    QString time = printList.at(3);
    QString add  = printList.at(4);

    myPrint->printConnect(net, id, type, time,add);
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
            setPrint(ui->tableWdt_record,m_myPrint,row,5);
        }
    }
    else if(index == 1)
    {
        int rowNum = ui->tableWdt_record->rowCount();
        if(rowNum > 0)
        {
            for(int row = 0;row < rowNum;row++)
            {
                setPrint(ui->tableWdt_record,m_myPrint,row,5);
            }
        }
    }
}

void Record::slotpBtnCurveCheck()
{
//    QString idStr = ui->lineEditCurve_id->text();
//    QString netStr = ui->lineEditCurve_net->text();
//    uint startTime = ui->dTEditCurve_start->dateTime().toTime_t();
//    uint endTime = ui->dTEditCurve_end->dateTime().toTime_t();
//    QString startTimeStr = QString::number(startTime);
//    QString endTimeStr   = QString::number(endTime);
//    QString results = m_db.getCurveData(netStr,idStr,startTimeStr,endTimeStr);
//    qDebug()<<results;
    //    GraphDemo(results);

}

void Record::slotpBtnCurveQuit()
{
    this->hide();
}
