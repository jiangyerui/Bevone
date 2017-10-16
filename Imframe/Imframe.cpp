

#include "Imframe.h"

IMFrame * IMFrame::im = NULL;

IMFrame::IMFrame()
{
    input=new MyselfInput(this);

    connect(input,SIGNAL(selfdel()),this,SLOT(slot_mydelete()));
    connect(input,SIGNAL(selfclose()),this,SLOT(slot_myclose()));
    connect(this,SIGNAL(hideclearbutton()),input,SLOT(slot_hideclearbutton()));
    connect(this,SIGNAL(showclearbtton()),input,SLOT(slot_showclearbutton()));

    x=10;
    y=10;
}

IMFrame::~IMFrame()
{
    delete input;
    delete currentwidget;
    delete control;
    deleteimframe();
}

IMFrame* IMFrame::getIMFrame()
{
    if(im==NULL)
        im = new IMFrame();
    return im;
}

void IMFrame::deleteimframe()
{
    if(im!=NULL)
    {
        delete im;
        im=NULL;
    }
}

void IMFrame::slot_myclose()
{
    input->hide();
}

void IMFrame::slot_mydelete()
{
    QWSServer::sendKeyEvent(0x01000003,Qt::Key_Backspace,Qt::NoModifier,true,false);
}

void IMFrame::sendContent(const QString& newcontent)
{
    content += newcontent;
    sendPreeditString(content,0);
    confirmContent();
}

void IMFrame::getcurrentwidget()
{
    currentwidget=QApplication::activeWindow();
    if(currentwidget)
    {
        control = currentwidget->focusWidget();

        if(control)
        {
            QString objName = control->objectName();
            //MainPage
            if("lineEdit"==objName)
            {
               x=170;
               y=260;
            }
            //login
            else if("lineEdit_passwd" == objName)
            {
                x=170;
                y=270;
            }
            //systemSet
            else if("lineEdit_day" == objName || "lineEdit_hour"==objName||
                    "lineEdit_year" == objName || "lineEdit_month"==objName||
                    "lineEdit_minute" == objName || "lineEdit_second"==objName||
                    "lineEdit_net" == objName  || "lineEdit_id"==objName||
                    "lineEdit_alarm" == objName|| "lineEdit_have"==objName||
                    "lineEdit_date" == objName || "lineEdit_times"==objName||
                    "lineEdit_tem" == objName  || "lineEdit_smallType"==objName||
                    "lineEdit_type" == objName)
            {
                x=170;
                y=250;
            }
            else if("lineEdit_dayNum" == objName || "lineEdit_pollTime"==objName||
                    "lineEdit_newPasswd" == objName || "lineEdit_oldPasswd" == objName)
            {
                x=170;
                y=100;
            }
            //Record
            else if("dTEdit_start" == objName || "dTEdit_end" == objName ||
                    "lineEdit_address" == objName || "lineEdit_pageNum" == objName ||
                    "lineEdit_pass" == objName)
            {
                x=170;
                y=150;
                emit hideclearbutton();
            }
            else
            {
                emit showclearbtton();
            }
            control=NULL;
        }
    }
}

void IMFrame::confirmContent()
{
    sendCommitString(content);
    content.clear();
}

void IMFrame::updateHandler(int type)
{

    switch(type)
    {
    case QWSInputMethod::FocusOut:
        content.clear();
        input->hide();
        break;
    case QWSInputMethod::FocusIn:
        content.clear();
        //获取到当前聚焦控件的位置
        getcurrentwidget();
        input->setGeometry(x,y,input->width(),input->height());
        input->show();
        currentwidget=NULL;
    default:
        break;
    }
}

void IMFrame::showselfinput(int type)
{
    switch(type)
    {
    case QWSInputMethod::FocusOut:
        content.clear();
        input->hide();
        break;
    case QWSInputMethod::FocusIn:
        content.clear();
        //获取到当前聚焦控件的位置 gz
        getcurrentwidget();
        input->setGeometry(x,y,input->width(),input->height());;
        input->show();
        currentwidget=NULL;
    default:
        break;
    }
}
