#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{

    init();
    initConnect();
}

void Manager::init()
{
    m_lockStatus = false;
    m_mainPage = new MainWindow;
    m_login    = new UserLogin;


}

void Manager::initConnect()
{
    connect(m_mainPage,SIGNAL(sigUserLogin()),m_login,SLOT(loginShow()));
    connect(m_mainPage,SIGNAL(sigLoginStatus(bool)),m_login,SLOT(slotLoginStatus(bool)));
    connect(m_login,SIGNAL(sigMainPageShow()),m_mainPage,SLOT(slotMainPageShow()));

    connect(m_login,SIGNAL(sigLoginStatus(int)),m_mainPage,SLOT(slotLoginStatus(int)));
}

void Manager::managerWork()
{
    //解锁状态  ///如果没有登陆，还是回到登陆界面
    if(!m_db.getLockStatus())
    {
        //qDebug()<<"not login -----";
        m_mainPage->hide();
        m_login->m_locked = true;
        m_login->loginShow();
        return;
    }

    //如果登陆了，就显示主页
    m_mainPage->show();
    m_login->m_locked = false;
    m_login->hide();

}

