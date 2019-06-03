#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QDebug>
#include "MainPage/mainwindow.h"
#include "UserLogin/userlogin.h"
#include "MySqlite/mysqlite.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);

    MainWindow *m_mainPage;
    UserLogin  *m_login;
    MySqlite    m_db;



    bool m_lockStatus;

    void init();
    void initConnect();
    void managerWork();

signals:

public slots:
};

#endif // MANAGER_H
