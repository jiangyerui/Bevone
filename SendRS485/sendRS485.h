#ifndef SENDRS485_H
#define SENDRS485_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "rs485.h"
#include "GlobalData/globaldata.h"
class SendRS485 : public QObject
{
    Q_OBJECT
public:
    explicit SendRS485(QObject *parent = 0);
    int initRS485(const char *devName);
    QTimer *m_timer;
    RS485 *m_rs485;
signals:

public slots:
    void slotTimeOut();
};

#endif // SENDRS485_H
