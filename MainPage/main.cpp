#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "MySqlite/mysqlite.h"
#include "Manager/manager.h"
#include "Imframe/Imframe.h"
#include "ScreenSaver/screensaver.h"


#define QUITTIME 180*1000

#define ON_ARM

//#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
//void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
//{
//    QString txt=str;
//#else
//void customMessageHandler(QtMsgType type, const char *msg)
//{
//    QString txt(msg);
//#endif
//    QFile outFile("debug.log");
//    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//    QTextStream ts(&outFile);
//    ts << txt << endl;
//}


int main(int argc, char *argv[])
{

    QTextCodec *codec = QTextCodec::codecForName("GBK");//设置成GBK是为了打印的时候能直接转换
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBk"));
    QApplication a(argc, argv,QApplication::GuiServer);

//#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
//    qInstallMessageHandler(customMessageHandler);
//#else
//    qInstallMsgHandler(customMessageHandler);
//#endif


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

    Manager manager;
    manager.managerWork();

    return a.exec();
}





