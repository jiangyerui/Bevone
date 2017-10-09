#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QWidget>
#include "MySqlite/mysqlite.h"
/*    主要功能
 * 1.操作员登录
 * 2.管理员登录
 * 3.超级用户登录
 * 4.密钥验证
 * 5.
 * 6.
 * 7.
 * 8.
 * 9.
 *
 *
*/


namespace Ui {
class UserLogin;
}

class UserLogin : public QWidget
{
    Q_OBJECT

public:
    explicit UserLogin(QWidget *parent = 0);
    ~UserLogin();

    MySqlite m_db;
    void initConfigure();

    bool m_locked;//是否锁定
    bool m_loginStatus;//登录状态

    QString m_user;
    QString m_admin;
    QString m_super;
    QString m_secretKey;

    void getLocked(bool locked);//获取是否已经锁定
    void initVar();


private:
    Ui::UserLogin *ui;

signals:
    void sigLoginStatus(int type);
    void sigMainPageShow();

public slots:
    void slotBtnOk();
    void slotBtnCancel();
    void loginShow();//初始化登录窗口
    void slotLoginStatus(bool loginStatus);

};

#endif // USERLOGIN_H
