#ifndef SMS_H
#define SMS_H
#include <QObject>
#include <QTimer>
#include <QString>
#include "LinuxCom/posix_qextserialport.h"
#include "LinuxCom/qextserialbase.h"
#include "SMS.h"
#include <QSerialPort>


class QtSMS : public QObject
{
    Q_OBJECT
public:
    explicit QtSMS(QObject *parent = 0);
    static QtSMS *getqtsms();

    bool OpenCom(const QString &name);
    QSerialPort *m_port;
    static QtSMS *qtsms;

    void SendSms(QString qStrSend);
};

#endif // SMS_H
