#ifndef MYSELFINPUT_H
#define MYSELFINPUT_H

#include <QWidget>
#include <QLineEdit>
#include <QSignalMapper>
#include <QDateTimeEdit>

class IMFrame;
namespace Ui {
    class MyselfInput;
}

class MyselfInput : public QWidget
{
    Q_OBJECT

public:
    explicit MyselfInput(IMFrame* im);
    ~MyselfInput();

private:
    Ui::MyselfInput *ui;

    QFont     font;
    IMFrame *im2;
    QWidget *widget;
    QWidget *control;
    QLineEdit *lineedit;
    QDateTimeEdit *findtime;
    QSignalMapper *signalmapper;

public slots:
    void input(QString str);
    void slot_myclear();
    void slot_hideclearbutton();
    void slot_showclearbutton();

signals:
    void selfdel();
    void selfclose();

};
#endif // MYSELFINPUT_H
