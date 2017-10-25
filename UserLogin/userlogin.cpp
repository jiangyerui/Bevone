#include "userlogin.h"
#include "ui_userlogin.h"
#include <QMessageBox>

UserLogin::UserLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(0,0,800,480);

    initVar();

    connect(ui->pBtn_ok,SIGNAL(clicked(bool)),this,SLOT(slotBtnOk()));
    connect(ui->pBtn_cancel,SIGNAL(clicked(bool)),this,SLOT(slotBtnCancel()));
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::initConfigure()
{
    if(!m_db.getLockStatus())
    {
        m_locked = true;
    }
}

void UserLogin::getLocked(bool locked)
{
    m_locked = locked;
}

void UserLogin::initVar()
{
    m_locked = FALSE;
    m_loginStatus = FALSE;

    m_user.clear();
    m_admin.clear();
    m_super.clear();
    m_secretKey.clear();

}

void UserLogin::loginShow()
{

    show();
    m_user  = m_db.getUserPasswd(MySqlite::USER);
    m_admin = m_db.getUserPasswd(MySqlite::ADMIN);
    m_super = m_db.getUserPasswd(MySqlite::SUPER);
    m_secretKey = m_db.getSecretKey();
    //qDebug()<<"m_secretKey = "<<m_secretKey;
    //获取序列号
    ui->lb_serialNO->setText(m_db.getSerialNumber());

    //获取外部版本

    //ui->lb_EVNValue->setText(m_db.getEVN());

    ui->lb_msg->setFocus();
    ui->lineEdit_passwd->clear();
    //已经上锁
    if(m_locked)
    {

        ui->lb_cue->show();
        ui->lineEdit_passwd->show();
        ui->lineEdit_passwd->setMaxLength(16);
        ui->lineEdit_passwd->setEchoMode(QLineEdit::Normal);

        ui->pBtn_cancel->setEnabled(false);
        ui->lb_cue->setText(tr("密 钥："));
        ui->lb_msg->setText(tr("由于设备上锁给您带来不便\n请尽快联系厂家售后人员..."));
        ui->lineEdit_passwd->setPlaceholderText(tr("输入解锁密钥"));
    }
    else
    {
        //未登录
        if(!m_loginStatus)
        {
            ui->lb_cue->show();
            ui->lineEdit_passwd->show();
            ui->lineEdit_passwd->setMaxLength(6);
            ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
            ui->pBtn_cancel->setEnabled(true);
            ui->lb_cue->setText(tr("密 码："));
            ui->lb_msg->setText(tr("欢迎您登录本系统\n请输入该设备的密码..."));
            ui->lineEdit_passwd->setPlaceholderText(tr("输入登录密码"));
        }
        else
        {
            //已经登录
            ui->lb_cue->hide();
            ui->lineEdit_passwd->hide();
            ui->lb_cue->setText(tr("密 码："));
            ui->pBtn_cancel->setEnabled(true);
            ui->lb_msg->setText(tr("点击-确定-退出登录状态\n点击-取消-保持登录状态"));
            ui->lineEdit_passwd->setPlaceholderText(tr("输入登录密码"));
        }
    }
}

void UserLogin::slotLoginStatus(bool loginStatus)
{
    m_loginStatus = loginStatus;
}

void UserLogin::slotBtnOk()
{

    if(m_locked)
    {
        QString passWd = ui->lineEdit_passwd->text();
        //qDebug()<<"passWd = "<<passWd;
        if(m_secretKey == passWd)
        {
            m_locked = false;
            m_db.setSuccess(TRUE);
            //m_db.setPastTime(0);
            //m_db.insertSecretKeyTime(0,0);
            emit sigMainPageShow();
            this->hide();
        }
    }
    else
    {
        if(!m_loginStatus)
        {
            QString passWd = ui->lineEdit_passwd->text();
            m_loginStatus = true;

            if(m_user == passWd)
            {
                emit sigLoginStatus(MySqlite::USER);
                this->hide();
            }
            else if(m_admin == passWd)
            {
                emit sigLoginStatus(MySqlite::ADMIN);
                this->hide();
            }
            else if(m_super == passWd)
            {
                emit sigLoginStatus(MySqlite::SUPER);
                this->hide();
            }
            else
            {
                m_loginStatus = false;
                QMessageBox::information(NULL,tr("信息提示"),tr("密码错误！"),tr("确定"));
            }
        }
        else
        {
            m_loginStatus = false;
            emit sigLoginStatus(MySqlite::QUIT);
            this->hide();
        }

    }


}

void UserLogin::slotBtnCancel()
{
    this->hide();
}
