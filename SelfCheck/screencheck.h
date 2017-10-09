#ifndef SCREENCHECK_H
#define SCREENCHECK_H

#include <QDialog>
#include <QMouseEvent>

#define RED    1
#define BLUE   2
#define GREEN  3
#define WHITE  4
#define YELLOW 5


namespace Ui {
class ScreenCheck;
}

class ScreenCheck : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenCheck(QWidget *parent = 0);
    ~ScreenCheck();
    void setColor(uint num);

private:
    Ui::ScreenCheck *ui;
protected:
    void mousePressEvent(QMouseEvent* e);

};

#endif // SCREENCHECK_H
