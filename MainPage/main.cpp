#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "MySqlite/mysqlite.h"
#include "Manager/manager.h"
#include "Imframe/Imframe.h"
//#include "ScreenSaver/screensaver.h"

#define QUITTIME 30*1000

//#define ON_ARM

int main(int argc, char *argv[])
{

    QTextCodec *codec = QTextCodec::codecForName("GBK");//设置成GBK是为了打印的时候能直接转换
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBk"));
    QApplication a(argc, argv,QApplication::GuiServer);

#ifdef ON_ARM
    IMFrame* im = IMFrame::getIMFrame();
    QWSServer::setCurrentInputMethod(im);

    ScreenSaver *screenSaver = new ScreenSaver();
    QWSServer::setScreenSaver(screenSaver);
    QWSServer::setScreenSaverInterval(QUITTIME);  //设置退出事件
#endif
    QFont font;
    font.setPointSize(15);

    MySqlite db;
    db.initDb();//初始化数据库
    db.delData("delete from TEMP;");//删除临时记录

    Manager manager;
    manager.managerWork();

    return a.exec();
}
