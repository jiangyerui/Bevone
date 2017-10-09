#include "screencheck.h"
#include "ui_screencheck.h"


ScreenCheck::ScreenCheck(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenCheck)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(0,0,800,480);
    ui->lb_check->setStyleSheet(QString("background-color: rgb(255, 0, 0)"));
}

ScreenCheck::~ScreenCheck()
{
    delete ui;
}

void ScreenCheck::setColor(uint num)
{
    switch (num) {
    case RED:
        ui->lb_check->setStyleSheet(QString("background-color: rgb(255, 0, 0)"));
        break;
    case BLUE:
        ui->lb_check->setStyleSheet(QString("background-color: rgb(0, 0, 255)"));
        break;
    case GREEN:
        ui->lb_check->setStyleSheet(QString("background-color: rgb(0, 255, 0)"));
        break;
    case WHITE:
        ui->lb_check->setStyleSheet(QString("background-color: rgb(255, 255, 255)"));
        break;
    case YELLOW:
        ui->lb_check->setStyleSheet(QString("background-color: rgb(255, 255, 0)"));
        break;
    default:
        break;
    }
}

void ScreenCheck::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    //this->hide();
}

