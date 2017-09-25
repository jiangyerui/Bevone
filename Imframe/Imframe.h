#ifndef IMFRAME_H
#define IMFRAME_H

#include <QRect>
#include <QDebug>
#include <QPoint>
#include <QWidget>
#include <QLineEdit>
#include <QWSInputMethod>
#include "Keyboard/Keyboard.h"

class IMFrame:public QWSInputMethod
{
    Q_OBJECT
public:
    ~IMFrame();
    void setMicroFocus(int, int);
    void updateHandler(int);
    void getcurrentwidget();
    QPoint getPoint(QRect qr,int w,int h);
    void my_show(QRect *location);
    void showselfinput(int type);
    static IMFrame * getIMFrame();
    static void deleteimframe();

protected:
    static IMFrame * im;

public slots:
    void sendContent(const QString&);
    void slot_myclose();
    void slot_mydelete();

private:
    IMFrame();
    void confirmContent();

    int x,y;
    QWidget *control;
    MyselfInput *input;
    QString content;
    QWidget *currentwidget;
signals:
    void hideclearbutton();
    void showclearbtton();
};

#endif // IMFRAME_H
