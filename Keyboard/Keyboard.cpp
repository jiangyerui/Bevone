#include "Keyboard.h"
#include "ui_Keyboard.h"
#include "Imframe/Imframe.h"
#include <QDebug>

MyselfInput::MyselfInput(IMFrame *im):QWidget(0,Qt::Tool| Qt::WindowStaysOnTopHint| Qt::FramelessWindowHint|Qt::Dialog),
    ui(new Ui::MyselfInput)
{
    ui->setupUi(this);
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    im2=im;
    this->setWindowOpacity(1);//设置透明度
    //traversalControl(this->children());

    signalmapper= new QSignalMapper(this);
    connect(ui->pBtn_1,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_2,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_3,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_4,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_5,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_6,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_7,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_8,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_9,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_10, SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_del,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_gb,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_a,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_b,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_c,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_d,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_e,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_f,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_g,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_h,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_i,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_j,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_k,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_l,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_m,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_n,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_o,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_p,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_q,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_r,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_s,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_t,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_u,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_v,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_w,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_x,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_y,SIGNAL(clicked()),signalmapper,SLOT(map()));
    connect(ui->pBtn_z,SIGNAL(clicked()),signalmapper,SLOT(map()));

    signalmapper->setMapping(ui->pBtn_1, ui->pBtn_1->text());
    signalmapper->setMapping(ui->pBtn_2, ui->pBtn_2->text());
    signalmapper->setMapping(ui->pBtn_3, ui->pBtn_3->text());
    signalmapper->setMapping(ui->pBtn_4, ui->pBtn_4->text());
    signalmapper->setMapping(ui->pBtn_5, ui->pBtn_5->text());
    signalmapper->setMapping(ui->pBtn_6, ui->pBtn_6->text());
    signalmapper->setMapping(ui->pBtn_7, ui->pBtn_7->text());
    signalmapper->setMapping(ui->pBtn_8, ui->pBtn_8->text());
    signalmapper->setMapping(ui->pBtn_9, ui->pBtn_9->text());
    signalmapper->setMapping(ui->pBtn_10,ui->pBtn_10->text());
    signalmapper->setMapping(ui->pBtn_del,ui->pBtn_del->text());
    signalmapper->setMapping(ui->pBtn_gb,ui->pBtn_gb->text());
    signalmapper->setMapping(ui->pBtn_a,ui->pBtn_a->text());
    signalmapper->setMapping(ui->pBtn_b,ui->pBtn_b->text());
    signalmapper->setMapping(ui->pBtn_c,ui->pBtn_c->text());
    signalmapper->setMapping(ui->pBtn_d,ui->pBtn_d->text());
    signalmapper->setMapping(ui->pBtn_e,ui->pBtn_e->text());
    signalmapper->setMapping(ui->pBtn_f,ui->pBtn_f->text());
    signalmapper->setMapping(ui->pBtn_g,ui->pBtn_g->text());
    signalmapper->setMapping(ui->pBtn_h,ui->pBtn_h->text());
    signalmapper->setMapping(ui->pBtn_i,ui->pBtn_i->text());
    signalmapper->setMapping(ui->pBtn_j,ui->pBtn_j->text());
    signalmapper->setMapping(ui->pBtn_k,ui->pBtn_k->text());
    signalmapper->setMapping(ui->pBtn_l,ui->pBtn_l->text());
    signalmapper->setMapping(ui->pBtn_m,ui->pBtn_m->text());
    signalmapper->setMapping(ui->pBtn_n,ui->pBtn_n->text());
    signalmapper->setMapping(ui->pBtn_o,ui->pBtn_o->text());
    signalmapper->setMapping(ui->pBtn_p,ui->pBtn_p->text());
    signalmapper->setMapping(ui->pBtn_q,ui->pBtn_q->text());
    signalmapper->setMapping(ui->pBtn_r,ui->pBtn_r->text());
    signalmapper->setMapping(ui->pBtn_s,ui->pBtn_s->text());
    signalmapper->setMapping(ui->pBtn_t,ui->pBtn_t->text());
    signalmapper->setMapping(ui->pBtn_u,ui->pBtn_u->text());
    signalmapper->setMapping(ui->pBtn_v,ui->pBtn_v->text());
    signalmapper->setMapping(ui->pBtn_w,ui->pBtn_w->text());
    signalmapper->setMapping(ui->pBtn_x,ui->pBtn_x->text());
    signalmapper->setMapping(ui->pBtn_y,ui->pBtn_y->text());
    signalmapper->setMapping(ui->pBtn_z,ui->pBtn_z->text());


    connect(signalmapper, SIGNAL(mapped(const QString &)),this,SLOT(input(QString)));

}

void MyselfInput::slot_hideclearbutton()
{
//    ui->pushButtonclear->setEnabled(false);
}

void MyselfInput::slot_showclearbutton()
{
//    ui->pushButtonclear->setEnabled(true);
}

void MyselfInput::slot_myclear()
{
    widget=QApplication::activeWindow();
    if(widget)
    {
        control=widget->focusWidget();
        lineedit= (QLineEdit *)control;
        lineedit->clear();
    }
    widget=NULL;
    control=NULL;
    lineedit=NULL;
}

void MyselfInput::input(QString str)
{
    if(tr("删除")==str)
    {
        emit selfdel();
    }
    else if(tr("关闭")==str)
    {
        emit selfclose();
    }
    else
    {
        im2->sendContent(str);
    }
}

MyselfInput::~MyselfInput()
{

    delete control;
    delete lineedit;
    delete widget;
    delete im2;

    delete signalmapper;
    delete ui;
}

