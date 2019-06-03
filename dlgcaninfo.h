#ifndef DLGCANINFO_H
#define DLGCANINFO_H

#include <QDialog>
#include "GlobalData/globaldata.h"


namespace Ui {
class DlgCanInfo;
}

class DlgCanInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCanInfo(QWidget *parent = 0);
    ~DlgCanInfo();

    QTimer *m_showTimer;
    int m_index;

    //一个探测器下的信息
      int m_curNet;//当前网络
    //探测器canId
//    int canId;
//    int alarmVal;//报警保持值
//    int curVal;//实漏电值
//    int setVal;//漏电报警
//    int guVal;//固有漏电值
    //制表
    void MakeHead();
    //初始化表的初值
    void initVal();
    //计算本探测器的所有值
    void culCanInfo(int index);
    //设置网络
    void setNet(int net);

private slots:
    void on_btn_Back_clicked();
    void slotShow();

private:
    Ui::DlgCanInfo *ui;
};

#endif // DLGCANINFO_H
