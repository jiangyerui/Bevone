#-------------------------------------------------
#
# Project created by QtCreator 2017-04-10T10:20:51
#
#-------------------------------------------------

QT       += core gui sql network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bevone
TEMPLATE = app
CONFIG += serialport

#DEFINES += RUN_ON_ARM


contains(DEFINES,RUN_ON_ARM){

SOURCES += MainPage/main.cpp\
        MainPage/mainwindow.cpp \
    SystemSet/systemset.cpp \
    Record/record.cpp \
    UserLogin/userlogin.cpp \
    SelfCheck/screencheck.cpp \
    MySqlite/mysqlite.cpp \
#    QCustomPlot/qcustomplot.cpp \
    LinuxCom/posix_qextserialport.cpp \
    LinuxCom/qextserialbase.cpp \
    MyPrint/myprint.cpp \
    Manager/manager.cpp \
    GlobalData/globaldata.cpp \
#    CanPort/canport.cpp \
#    CanRxThread/canrxthread.cpp \
#    CanTxThread/cantxthread.cpp \
    GpioControl/gpiocontrol.cpp \
    watchdog/watchdog.cpp \
    GpioChip/gpiochip.cpp \
#    SendRS485/rs485.cpp \
#    SendRS485/sendRS485.cpp \
    TcpServer/tcpserver.cpp \
    Keyboard/Keyboard.cpp \
    Imframe/Imframe.cpp \
    CalculaNode/calculanode.cpp \
#    SMS/SMS.cpp \
#    SerialZigBee/serialhandle.cpp \
    CanMoudle/canmoudle.cpp\
    ScreenSaver/screensaver.cpp

HEADERS  += MainPage/mainwindow.h \
    SystemSet/systemset.h \
    Record/record.h \
    UserLogin/userlogin.h \
    SelfCheck/screencheck.h \
    MySqlite/mysqlite.h \
#    QCustomPlot/qcustomplot.h \
    LinuxCom/posix_qextserialport.h \
    LinuxCom/qextserialbase.h \
    MyPrint/myprint.h \
    Manager/manager.h \
    GlobalData/globaldata.h \
#    CanPort/canport.h \
#    CanRxThread/canrxthread.h \
#    CanTxThread/cantxthread.h \
    GpioControl/gpiocontrol.h \
    watchdog/watchdog.h \
    GpioChip/gpiochip.h \
#    SendRS485/rs485.h \
#    SendRS485/sendRS485.h \
    TcpServer/tcpserver.h \
    Keyboard/Keyboard.h \
    Imframe/Imframe.h \
    CalculaNode/calculanode.h \
#    SMS/SMS.h \
#    SerialZigBee/serialhandle.h \
    CanMoudle/canmoudle.h\
    ScreenSaver/screensaver.h

FORMS    += MainPage/mainwindow.ui \
    SystemSet/systemset.ui \
    Record/record.ui \
    UserLogin/userlogin.ui \
    SelfCheck/screencheck.ui \
    Keyboard/Keyboard.ui
}else{

SOURCES += MainPage/main.cpp\
        MainPage/mainwindow.cpp \
    SystemSet/systemset.cpp \
    Record/record.cpp \
    UserLogin/userlogin.cpp \
    SelfCheck/screencheck.cpp \
    MySqlite/mysqlite.cpp \
#    QCustomPlot/qcustomplot.cpp \
    LinuxCom/posix_qextserialport.cpp \
    LinuxCom/qextserialbase.cpp \
    MyPrint/myprint.cpp \
    Manager/manager.cpp \
    GlobalData/globaldata.cpp \
#    CanPort/canport.cpp \
#    CanRxThread/canrxthread.cpp \
#    CanTxThread/cantxthread.cpp \
    GpioControl/gpiocontrol.cpp \
    watchdog/watchdog.cpp \
    GpioChip/gpiochip.cpp \
#    SendRS485/rs485.cpp \
#    SendRS485/sendRS485.cpp \
    TcpServer/tcpserver.cpp \
    CalculaNode/calculanode.cpp \
#    SMS/SMS.cpp \
#    SerialZigBee/serialhandle.cpp \
    CanMoudle/canmoudle.cpp


HEADERS  += MainPage/mainwindow.h \
    SystemSet/systemset.h \
    Record/record.h \
    UserLogin/userlogin.h \
    SelfCheck/screencheck.h \
    MySqlite/mysqlite.h \
#    QCustomPlot/qcustomplot.h \
    LinuxCom/posix_qextserialport.h \
    LinuxCom/qextserialbase.h \
    MyPrint/myprint.h \
    Manager/manager.h \
    GlobalData/globaldata.h \
#    CanPort/canport.h \
#    CanRxThread/canrxthread.h \
#    CanTxThread/cantxthread.h \
    GpioControl/gpiocontrol.h \
    watchdog/watchdog.h \
    GpioChip/gpiochip.h \
#    SendRS485/rs485.h \
#    SendRS485/sendRS485.h \
    TcpServer/tcpserver.h \
    CalculaNode/calculanode.h \
#    SMS/SMS.h \
#    SerialZigBee/serialhandle.h \
    CanMoudle/canmoudle.h

FORMS    += MainPage/mainwindow.ui \
    SystemSet/systemset.ui \
    Record/record.ui \
    UserLogin/userlogin.ui \
    SelfCheck/screencheck.ui

}



RESOURCES += \
    image.qrc


