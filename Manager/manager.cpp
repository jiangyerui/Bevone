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
    //解锁状态
    if(!m_db.getLockStatus())
    {
        m_mainPage->hide();
        m_login->m_locked = true;
        m_login->loginShow();
        return;
    }

    m_mainPage->show();
    m_login->m_locked = false;
    m_login->hide();

}

