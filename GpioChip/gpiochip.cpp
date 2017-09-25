#include "gpiochip.h"
#include <QDebug>

/*
            𝑮𝑷𝑰𝑶排列序号=(𝑩𝑨𝑵𝑲−𝟏)×𝟑𝟐+𝑵
在公式中BANK为GPIO引脚所在的BANK，N则为引脚在该个BANK中的序号。
P3.4为例，其BANK值为3，N值为4，因此排列序号为(3-1)*32+4=68。
控制输出
GPIO2_09    GPIO41    报警
GPIO2_10    GPIO42    故障
GPIO2_11    GPIO43    通讯
GPIO2_12    GPIO44    备电正常
GPIO2_13    GPIO45    备电故障
GPIO2_14    GPIO46    主电正常
GPIO2_15    GPIO47    主电故障
控制输入
GPIO2_08    GPIO40    复位信号高电位
GPIO5_09    GPIO137   主电信号
GPIO5_05    GPIO133   备电信号
GPIO3_04    GPIO68    备电短路
继电器K1,K2 高电平导通
GPIO4_17    GPIO113    继电器1
GPIO4_19    GPIO115    继电器2


*/

const char *exportPath = "/sys/class/gpio/export";




GpioChip::GpioChip()
{
    m_fdExport = -1;
    m_fdDir    = -1;
    m_fdVal    = -1;
    m_ret      = -1;
}
int GpioChip::setState(const char *state)
{
    m_ret = write(m_fdDir, state, strlen(state)); //控制输入/输出

    if(m_ret < 0) {
        //qDebug()<<"write direction";
        return -1;
    }
    return 1;
}

void GpioChip::initGPIOChip(const char *dev, const char *devValue, const char *devDirection)
{
    //const char *exportPath = "/sys/class/gpio/export";
    m_fdExport = open(exportPath, O_WRONLY);    // 打开GPIO设备导出设备
    //写入控制GPIO编号
    write(m_fdExport, dev, strlen(dev));

    //打开输入输出电平值设备
    m_fdVal = open(devValue, O_RDWR);

    //打开GPIO输入输出控制设备
    m_fdDir = open(devDirection, O_RDWR);
}

int GpioChip::GPIOWrite(const char *buf)
{
    int ret = write(m_fdVal, buf, sizeof(buf));	// 写输入电平值
    if(ret < 0) {
        //qDebug()<<"write gpio";
        return -1;
    }
    return 1;
}

int GpioChip::GPIORead(char *buf,const char *devValue)
{
    int fdVal = open(devValue, O_RDWR);
    int ret = read(fdVal, buf, sizeof(buf));	// 读出电平值
    if(ret < 0) {
        return -1;
    }
    ::close(fdVal);
    return 1;
}

