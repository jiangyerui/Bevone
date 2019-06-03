#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "MySqlite/mysqlite.h"
#include "Manager/manager.h"
#include "Imframe/Imframe.h"
#include "ScreenSaver/screensaver.h"


#define QUITTIME 3*60*1000

#define ON_ARM


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

    MySqlite db;
    db.initDb();//初始化数据库
    db.delData("delete from TEMP;");//删除临时记录

    Manager manager;//为什么不创建对象就可以使用方法，又不是类方法，就算是类方法，也得通过类调用啊？？
    manager.managerWork();


    return a.exec();
}





