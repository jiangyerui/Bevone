#include "dlgcaninfo.h"
#include "ui_dlgcaninfo.h"
#include <stdio.h>



DlgCanInfo::DlgCanInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCanInfo)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    MakeHead();
    //initVal();

    //刷新详细信息界面
    m_showTimer = new QTimer;
    connect(m_showTimer,SIGNAL(timeout()),this,SLOT(slotShow()));
    m_showTimer->setInterval(1000);


}

DlgCanInfo::~DlgCanInfo()
{
    delete ui;
}
void DlgCanInfo::MakeHead(){
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);//不可选择
    //    ui->tableWidget->setRowCount(10);
    //    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount(8);
    ui->tableWidget->setColumnCount(6);
    QFont fontTable;
    fontTable.setBold(true);
    fontTable.setPixelSize(16);
    ui->tableWidget->setFont(fontTable);
    QStringList header;
    //    header<<tr("编号")<<tr("名称")<<tr("状态")<<tr("管理");
    header<<tr("编号")<<tr("类型")<<tr("报警值")<<tr("实时值")<<tr("设定值")<<tr("固有值");

    ui->tableWidget->setHorizontalHeaderLabels(header);
    //    ui->tableWidget->setColumnWidth(0,60);
    //    ui->tableWidget->setColumnWidth(1,280);
    //    ui->tableWidget->setColumnWidth(2,100);
    //    ui->tableWidget->setColumnWidth(3,97);
    ui->tableWidget->setColumnWidth(0,100);
    ui->tableWidget->setColumnWidth(1,120);
    ui->tableWidget->setColumnWidth(2,120);
    ui->tableWidget->setColumnWidth(3,120);
    ui->tableWidget->setColumnWidth(4,120);
    ui->tableWidget->setColumnWidth(5,120);

    QHeaderView* headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    //    ui->tableWidget->horizontalHeader()->setFixedHeight(48);
    ui->tableWidget->horizontalHeader()->setFixedHeight(48);
    ui->tableWidget->horizontalHeader()->setFont(fontTable);
    //    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能单选
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    //    nowBeginIndex=0;
    //    pageListSize=10;
}
//初始化初值
void DlgCanInfo::initVal(){

    for(int i=0;i<8;i++){
        //编码
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(tr("--")));
        //类型
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(tr("--")));
        //报警值
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(tr("--")));
        ui->tableWidget->item(i,2)->setBackgroundColor(Qt::yellow);
        //实时值
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(tr("--")));
        //设定值
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(tr("--")));
        //固有值
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(tr("--")));
    }

}
//计算本探测器的所有信息数据
void DlgCanInfo::culCanInfo(int index){
    m_index = index;
    bool isAlarm = false;
    int type=0;
    uint alarmValTemp=0;
    uint curValTemp=0;
    uint setValTemp=0;
    uint guValTemp=0;

    char channelNo[100];
    char alarmVal[100];
    char curVal[100];
    char setVal[100];
    char guVal[100];

    sprintf(alarmVal,"%01d",0);
    sprintf(curVal,"%01d",0);
    sprintf(setVal,"%01d",0);
    sprintf(guVal,"%01d",0);

    ui->tableWidget->clearContents();
    for(int i=1;i<9;i++){
        int node = (m_index-1)*8+i;
        if(mod[m_curNet][node].used==true){ //如果本回路有效
            //计算编码
            sprintf(channelNo,"%02d",i);
            ui->tableWidget->setItem(i-1,0,new QTableWidgetItem(tr("%1").arg(channelNo)));

            //计算类型
            type = mod[m_curNet][node].type;
            switch (type) {
            case MODULE_CUR:
                ui->tableWidget->setItem(i-1,1,new QTableWidgetItem(tr("漏电")));

                //计算漏电报警值
                isAlarm = mod[m_curNet][node].alarmFlag;//取报警标志位看现在是否在报警
                if(isAlarm){//如果当前回路报警
                    sprintf(alarmVal,"%01d",0);
                    alarmValTemp = mod[m_curNet][node].alarmData;//取出报警值
                    sprintf(alarmVal,"%01d",alarmValTemp);
                    ui->tableWidget->setItem(i-1,2,new QTableWidgetItem(tr("%1 mA").arg(alarmVal)));
                    ui->tableWidget->item(i-1,2)->setBackgroundColor(Qt::red);
                }else{//如果当前回路没报警
                    if(mod[m_curNet][node].dropFlag||mod[m_curNet][node].errorFlag)
                    {
                        ui->tableWidget->setItem(i-1,2,new QTableWidgetItem(tr("%1 mA").arg(alarmVal)));
                        ui->tableWidget->item(i-1,2)->setBackgroundColor(Qt::yellow);
                    }else{
                        ui->tableWidget->setItem(i-1,2,new QTableWidgetItem(tr("%1 mA").arg(alarmVal)));
                        ui->tableWidget->item(i-1,2)->setBackgroundColor(Qt::green);
                    }
                }
                //计算漏电实时值
                curValTemp = mod[m_curNet][node].rtData;
                //                qDebug()<<"curValTemp = "<<curValTemp;
                if(curValTemp!=0)
                    sprintf(curVal,"%01d",curValTemp);
                else
                    sprintf(curVal,"%01d",0);
                ui->tableWidget->setItem(i-1,3,new QTableWidgetItem(tr("%1 mA").arg(curVal)));
                //计算漏电设定值
                setValTemp = mod[m_curNet][node].alarmDataSet;
                if(setValTemp!=0)
                    sprintf(setVal,"%01d",setValTemp);
                else
                    sprintf(setVal,"%01d",300);
                ui->tableWidget->setItem(i-1,4,new QTableWidgetItem(tr("%1 mA").arg(setVal)));
                //计算漏电固有值
                guValTemp = mod[m_curNet][node].baseData;
                if(guValTemp!=0)
                    sprintf(guVal,"%01d",guValTemp);
                else
                    sprintf(guVal,"%01d",0);
                ui->tableWidget->setItem(i-1,5,new QTableWidgetItem(tr("%1 mA").arg(guVal)));

                break;
            case MODULE_TEM:
                ui->tableWidget->setItem(i-1,1,new QTableWidgetItem(tr("温度")));

                //计算温度报警值
                isAlarm = mod[m_curNet][node].alarmFlag;//取报警标志位看现在是否在报警
                if(isAlarm){//如果当前回路报警
                    sprintf(alarmVal,"%01d",0);
                    alarmValTemp = mod[m_curNet][node].alarmTem;//取出报警值
                    sprintf(alarmVal,"%01d",alarmValTemp);
                    ui->tableWidget->setItem(i-1,2,new QTableWidgetItem(tr("%1 ℃").arg(alarmVal)));
                    ui->tableWidget->item(i-1,2)->setBackgroundColor(Qt::red);
                }else{//如果当前回路没报警
                    if(mod[m_curNet][node].dropFlag||mod[m_curNet][node].errorFlag){
                        ui->tableWidget->setItem(i-1,2,new QTableWidgetItem(tr("%1 ℃").arg(alarmVal)));
                        ui->tableWidget->item(i-1,2)->setBackgroundColor(Qt::yellow);
                    }else{
                        ui->tableWidget->setItem(i-1,2,new QTableWidgetItem(tr("%1 ℃").arg(alarmVal)));
                        ui->tableWidget->item(i-1,2)->setBackgroundColor(Qt::green);
                    }

                }
                //计算温度实时值
                curValTemp = mod[m_curNet][node].temData;
                if(curValTemp!=0)
                    sprintf(curVal,"%01d",curValTemp);
                else
                    sprintf(curVal,"%01d",0);
                ui->tableWidget->setItem(i-1,3,new QTableWidgetItem(tr("%1 ℃").arg(curVal)));
                //计算温度设定值
                setValTemp = mod[m_curNet][node].alarmTemSet;
                if(setValTemp!=0)
                    sprintf(setVal,"%01d",setValTemp);
                else
                    sprintf(setVal,"%01d",55);
                ui->tableWidget->setItem(i-1,4,new QTableWidgetItem(tr("%1 ℃").arg(setVal)));
                //温度无固有值
                ui->tableWidget->setItem(i-1,5,new QTableWidgetItem(tr("--")));

                break;
            default:
                break;
            }

        }//if


    }//for

}
//设置网络
void DlgCanInfo::setNet(int net){
    this->m_curNet = net;
    initVal();
    m_showTimer->start();
}

void DlgCanInfo::on_btn_Back_clicked()
{
    m_showTimer->stop();
    this->close();
}
void DlgCanInfo::slotShow(){
    //    qDebug()<<"slotSShow***";
    culCanInfo(m_index);
}
